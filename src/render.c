#include "render.h"
#include "score.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *render_line(Renderer *renderer, Score *score, const bool selected) {
  // this function:
  // - highlights match
  // - expands tabs
  // - truncates line to width
  const size_t strlen_line = strlen(score->line);
  const size_t length = strlen_line +
                        (count_chars_in_string(score->line, '\t') * 8) +
                        strlen(COLOR_RESET COLOR_REVERSE COLOR_RED COLOR_DEFAULT
                                   COLOR_RESET CLEAR_LINE) +
                        3; // + 2 for \r\n, + 1 for \0
  char *rv = calloc(length, sizeof(char));
  size_t cursor = strlen(COLOR_RESET);
  size_t visible_chars = 0;
  strcat(rv, COLOR_RESET);
  if (selected) {
    strcat(rv, COLOR_REVERSE);
    cursor += strlen(COLOR_REVERSE);
  }
  for (size_t i = 0; i < strlen_line; i++) {
    if (score->first != score->last) {
      if (score->first == i) {
        strcat(rv, COLOR_RED);
        cursor += strlen(COLOR_RED);
      } else if (score->last == i) {
        strcat(rv, COLOR_DEFAULT);
        cursor += strlen(COLOR_DEFAULT);
      }
    }
    if (score->line[i] == '\t') {
      // expand tabs into spaces (actual tabs break reversed highlighting)
      do {
        rv[cursor++] = ' ';
        visible_chars++;
      } while (renderer->width > visible_chars && visible_chars % 8 != 0);
    } else if (renderer->width > visible_chars) {
      rv[cursor++] = score->line[i];
      visible_chars++;
    }
    if (renderer->width <= visible_chars) {
      // truncate at width of screen
      break;
    }
  }
  if (selected) {
    strcat(rv, COLOR_RESET);
  }
  strcat(rv, CLEAR_LINE "\r\n");
  return rv;
}

Renderer *renderer_new() {
  Renderer *r = calloc(1, sizeof(Renderer));
  return r;
}

char *renderer_render(Renderer *r) {
  char *rv = calloc(16384, sizeof(char)); // adjust this later?
  strcat(rv, HIDE_CURSOR "\r\n");
  Score *score;
  for (size_t i = 0; i < r->height - 1; i++) {
    if (r->scores->length > i) {
      score = r->scores->items[i];
      char *line = render_line(r, score, r->selected == i);
      strcat(rv, line);
      free(line);
    } else {
      strcat(rv, CLEAR_LINE "\r\n");
    }
  }
  char clear[10] = "";
  memset(clear, 0, 10);
  sprintf(clear, "\e[%zdA", (size_t)r->height);
  strcat(rv, clear);
  char prompt[r->width];
  memset(prompt, 0, r->width);
  snprintf(prompt, r->width, "%*zd > %s", r->match_length, r->scores->length,
           r->query);
  strcat(rv, prompt);
  strcat(rv, SHOW_CURSOR CLEAR_LINE);
  return rv;
}
