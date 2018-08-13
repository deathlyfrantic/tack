#include "config.h"
#include "hash-table.h"
#include "list.h"
#include "render.h"
#include "score.h"
#include "tack-string.h"
#include "tack.h"
#include "tty.h"
#include "util.h"
#include <ctype.h>
#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static bool parse_args(int argc, char *argv[], Config *config) {
  static const struct option longopts[] = {
    { "height", required_argument, NULL, 'H' },
    { "search", required_argument, NULL, 's' },
    { "version", no_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { NULL, 0, NULL, 0 }
  };
  static const char usage[] =
    "Usage: %s [options]\n"
    "  -H, --height <lines>   Specify UI height in lines (including "
    "prompt).\n"
    "                         (Use `--height full` for full screen)\n"
    "  -s, --search <search>  Specify an initial search string\n"
    "  -h, --help\n"
    "  -v, --version\n";
  int c;
  while ((c = getopt_long(argc, argv, "H:s:hv", longopts, NULL)) != -1) {
    switch (c) {
    case 'H':
      if (strcmp(optarg, "full") == 0) {
        config->full_height = true;
      } else {
        sscanf(optarg, "%" SCNu16, &config->height);
      }
      break;
    case 's':
      free(config->initial_search);
      config->initial_search = strdup(optarg);
      break;
    case 'v': // fallthrough
      puts(VERSION);
      return true;
    case 'h': // fallthrough
    default:
      printf(usage, argv[0]);
      return true;
    }
  }
  return false;
}

static List *get_lines_from_stdin() {
  String *s = string_new();
  char c;
  List *lines = list_new_of_size(100);
  bool keep_going = true;
  flockfile(stdin);
  do {
    switch ((c = getc_unlocked(stdin))) {
    case EOF:
      keep_going = false;
      if (string_equals(s, "")) {
        break;
      }
      // possible fallthrough here is intentional
    case '\n':
      list_push(lines, s);
      s = string_new();
      break;
    default:
      string_push_char(s, c);
      break;
    }
  } while (keep_going);
  funlockfile(stdin);
  return lines;
}

static void free_scores(void *ptr) {
  if (ptr == NULL) return;
  List *scores = (List *)ptr;
  for (size_t i = 0; i < scores->length; i++) {
    free(scores->items[i]);
  }
  free(scores);
}

static List *find_closest_scores(HashTable *table, String *query) {
  List *scores = NULL;
  char tmp[query->length + 1];
  strcpy(tmp, query->buf);
  size_t cursor = query->length;
  while (cursor--) {
    tmp[cursor] = '\0';
    scores = hashtable_get(table, tmp);
    if (scores != NULL) {
      break;
    }
  }
  return scores;
}

static List *calculate_scores(List *old, String *query) {
  List *scores = list_new_of_size(old->length);
  Score *old_score, *score;
  for (size_t i = 0; i < old->length; i++) {
    old_score = old->items[i];
    score = calculate_score(old_score->line, query);
    if (score != NULL) {
      list_push(scores, score);
    }
  }
  list_sort_by_score(scores);
  return scores;
}

static bool run_main_loop(List *initial_scores, Config *config) {
  bool killed = false;
  String *query = string_new_from(config->initial_search);
  bool need_new_scores = query->length > 0;
  size_t selected = 0;
  TTY *tty = tty_new();
  Renderer renderer = {
    .match_length = get_num_strlen(initial_scores->length),
    .width = tty->columns,
    .height = MIN(
      config->full_height ? tty->rows - 1 : MIN(config->height, tty->rows - 1),
      // "clamp" renderer height if there aren't enough input lines to fill it
      initial_scores->length + 1)
  };
  Score *score;
  char c;
  HashTable *table = hashtable_new();
  hashtable_set(table, "", initial_scores);
  List *scores = initial_scores;
  while (true) {
    if (need_new_scores) {
      scores = hashtable_get(table, query->buf);
      if (scores == NULL) {
        scores = calculate_scores(find_closest_scores(table, query), query);
        hashtable_set(table, query->buf, scores);
      }
      need_new_scores = false;
    }
    selected = MIN(selected, scores->length - 1);
    renderer.query = query->buf;
    renderer.selected = selected;
    renderer.scores = scores;
    String *output = renderer_render(&renderer);
    tty_write(tty, output);
    string_free(output);
    switch ((c = tty_read_char(tty))) {
    case CTRL_KEY('c'):
      killed = true;
      // intentional fallthrough
    case '\r': // enter
      goto exit;
    case CTRL_KEY('n'):
      if (selected < MIN(renderer.height - 2, scores->length - 1)) {
        selected++;
      } else {
        selected = 0;
      }
      break;
    case CTRL_KEY('p'):
      if (selected > 0) {
        selected--;
      } else {
        selected = renderer.height - 2;
      }
      break;
    case CTRL_KEY('w'):
      string_delete_word(query);
      need_new_scores = true;
      break;
    case CTRL_KEY('u'):
      string_reset(query);
      need_new_scores = true;
      break;
    case 127: // backspace
      if (query->length > 0) {
        string_pop_char(query);
        need_new_scores = true;
      }
      break;
    default:
      if (isprint(c)) {
        string_push_char(query, c);
        // don't bother calculating new scores if there are currently 0 matches
        need_new_scores = scores->length > 0;
      }
      break;
    }
  }
exit:;
  // clear all extant output
  String *clear = string_new_from(COLOR_RESET CLEAR_WHOLE_LINE "\r\n");
  for (size_t i = 0; i < renderer.height; i++) {
    tty_write(tty, clear);
  }
  string_free(clear);
  tty_teardown_and_free(tty);
  if (!killed) {
    // write selected line to stdout
    score = scores->items[selected];
    puts(score->line->buf);
  }
  hashtable_free_items(table, free_scores);
  hashtable_free(table);
  return killed;
}

#ifndef TESTS

int main(int argc, char *argv[]) {
  bool killed = false;
  Config *config = config_new();
  if (parse_args(argc, argv, config)) {
    goto exit;
  }
  List *lines = get_lines_from_stdin();
  List *scores = list_new_of_size(lines->length);
  Score *score;
  String *empty = string_new();
  for (size_t i = 0; i < lines->length; i++) {
    score = calculate_score(lines->items[i], empty);
    list_push(scores, score);
  }
  string_free(empty);
  killed = run_main_loop(scores, config);
  for (size_t i = 0; i < lines->length; i++) {
    string_free(lines->items[i]);
  }
  free(lines);
exit:
  config_free(config);
  if (killed) {
    // kill all processes in the chain if cancelled
    killpg(getpgrp(), SIGINT);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

#endif
