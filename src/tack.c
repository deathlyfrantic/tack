#include "hash-table.h"
#include "list.h"
#include "render.h"
#include "score.h"
#include "tack.h"
#include "tty.h"
#include "util.h"
#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static Config *config_init() {
  Config *config = malloc(sizeof(Config));
  config->full_height = false;
  config->height = 21;
  config->initial_search = strdup("");
  return config;
}

static void config_free(Config *config) {
  free(config->initial_search);
  free(config);
}

static bool parse_args(int argc, char *argv[], Config *config) {
  static struct option longopts[] = {{"height", required_argument, NULL, 'H'},
                                     {"search", required_argument, NULL, 's'},
                                     {"version", no_argument, NULL, 'v'},
                                     {"help", no_argument, NULL, 'h'},
                                     {NULL, 0, NULL, 0}};
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
        config->height = strtol(optarg, NULL, 10);
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
  char line[BUFSIZ];
  unsigned int cursor = 0;
  char c;
  List *lines = list_new_of_size(100);
  bool keep_going = true;
  flockfile(stdin);
  do {
    switch ((c = getc_unlocked(stdin))) {
    case EOF:
      keep_going = false;
      if (strcmp(line, "") == 0) {
        break;
      }
      // possible fallthrough here is intentional
    case '\n':
      line[cursor] = '\0';
      cursor = 0;
      list_push(lines, strdup(line));
      memset(line, 0, sizeof(char) * BUFSIZ);
      break;
    default:
      line[cursor++] = c;
      break;
    }
  } while (keep_going);
  funlockfile(stdin);
  return lines;
}

static void free_scores(void *ptr) {
  List *scores = (List *)ptr;
  for (size_t i = 0; i < scores->length; i++) {
    free(scores->items[i]);
  }
  free(scores);
}

static List *find_closest_scores(HashTable *table, const char *query) {
  List *scores = NULL;
  size_t strlen_query = strlen(query);
  char tmp[strlen_query + 1];
  strcpy(tmp, query);
  size_t cursor = strlen_query;
  while (cursor--) {
    tmp[cursor] = '\0';
    scores = hashtable_get(table, tmp);
    if (scores != NULL) {
      break;
    }
  }
  return scores;
}

static List *calculate_scores(List *old, const char *query) {
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
  char query[BUFSIZ];
  memset(query, 0, BUFSIZ);
  strcat(query, config->initial_search);
  size_t cursor = strlen(query);
  size_t selected = 0;
  TTY *tty = tty_new();
  Renderer *renderer = renderer_new();
  renderer->match_length = get_num_strlen(initial_scores->length);
  renderer->height =
      config->full_height ? tty->rows - 1 : MIN(config->height, tty->rows - 1);
  renderer->width = tty->columns;
  Score *score;
  char c;
  bool need_new_scores = strlen(query) > 0;
  HashTable *table = hashtable_new();
  hashtable_set(table, "", initial_scores);
  List *scores = initial_scores;
  while (true) {
    if (need_new_scores) {
      scores = hashtable_get(table, query);
      if (scores == NULL) {
        scores = calculate_scores(find_closest_scores(table, query), query);
        hashtable_set(table, query, scores);
      }
      need_new_scores = false;
    }
    selected = MIN(selected, scores->length - 1);
    renderer->query = query;
    renderer->selected = selected;
    renderer->scores = scores;
    char *output = renderer_render(renderer);
    tty_write(tty, output);
    free(output);
    switch ((c = tty_read_char(tty))) {
    case CTRL_KEY('c'):
      killed = true;
      // intentional fallthrough
    case '\r': // enter
      goto exit;
    case CTRL_KEY('n'):
      if (selected < renderer->height - 2) {
        selected++;
      } else {
        selected = 0;
      }
      break;
    case CTRL_KEY('p'):
      if (selected > 0) {
        selected--;
      } else {
        selected = renderer->height - 2;
      }
      break;
    case CTRL_KEY('w'): {
      bool seen_char = false;
      while (true) {
        if (cursor == 0 || !isspace(query[cursor - 1])) {
          seen_char = true;
        }
        query[cursor] = '\0';
        if (cursor == 0 || (isspace(query[cursor - 1]) && seen_char)) {
          need_new_scores = true;
          break;
        }
        cursor--;
      }
      break;
    }
    case CTRL_KEY('u'):
      memset(query, 0, sizeof(char) * BUFSIZ);
      cursor = 0;
      need_new_scores = true;
      break;
    case 127: // backspace
      if (cursor > 0) {
        query[--cursor] = '\0';
        need_new_scores = true;
      }
      break;
    default:
      if (isprint(c)) {
        query[cursor++] = c;
        query[cursor] = '\0';
        need_new_scores = true;
      }
      break;
    }
  }
exit:
  // clear all extant output
  for (size_t i = 0; i < renderer->height; i++) {
    tty_write(tty, COLOR_RESET CLEAR_WHOLE_LINE "\r\n");
  }
  tty_teardown_and_free(tty);
  if (!killed) {
    // write selected line to stdout
    score = scores->items[selected];
    puts(score->line);
  }
  hashtable_free_items(table, free_scores);
  hashtable_free(table);
  free(renderer);
  return killed;
}

int main(int argc, char *argv[]) {
  bool killed = false;
  Config *config = config_init();
  if (parse_args(argc, argv, config)) {
    goto exit;
  }
  List *lines = get_lines_from_stdin();
  List *scores = list_new_of_size(lines->length);
  Score *score;
  for (size_t i = 0; i < lines->length; i++) {
    score = calculate_score(lines->items[i], "");
    list_push(scores, score);
  }
  killed = run_main_loop(scores, config);
  for (size_t i = 0; i < lines->length; i++) {
    free(lines->items[i]);
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
