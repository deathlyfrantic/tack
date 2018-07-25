#include "list.h"
#include "render.h"
#include "score.h"
#include "tack.h"
#include "tty.h"
#include "util.h"
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

static void free_scores(List *scores) {
  for (size_t i = 0; i < scores->length; i++) {
    free(scores->items[i]);
  }
  free(scores);
}

static bool run_main_loop(List *stdin_lines) {
  bool killed = false;
  char query[BUFSIZ];
  memset(query, 0, BUFSIZ);
  size_t cursor = 0;
  size_t selected = 0;
  TTY *tty = tty_new();
  Renderer *renderer = renderer_new();
  renderer->match_length = get_num_strlen(stdin_lines->length);
  renderer->height = MIN(15, tty->rows - 1);
  Score *score;
  char c;
  bool need_new_scores = true;
  List *scores;
  while (true) {
    if (need_new_scores) {
      scores = list_new_of_size(100);
      for (size_t i = 0; i < stdin_lines->length; i++) {
        score = calculate_score(stdin_lines->items[i], query);
        if (score != NULL) {
          list_push(scores, score);
        }
      }
      list_sort_by_score(scores);
    }
    need_new_scores = false;
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
      }
      break;
    case CTRL_KEY('p'):
      if (selected > 0) {
        selected--;
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
    if (need_new_scores) {
      free_scores(scores);
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
  free_scores(scores);
  free(renderer);
  return killed;
}

int main(int argc, char *argv[]) {
  List *lines = get_lines_from_stdin();
  bool killed = run_main_loop(lines);
  for (size_t i = 0; i < lines->length; i++) {
    free(lines->items[i]);
  }
  free(lines);
  if (killed) {
    // kill all processes in the chain if cancelled
    killpg(getpgrp(), SIGINT);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
