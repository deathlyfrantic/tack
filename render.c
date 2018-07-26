#include "render.h"
#include "score.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *render_line(const char *s, size_t beg, size_t end, bool selected,
                  bool has_query) {
  const size_t strlen_s = strlen(s);
  size_t length = strlen_s + (count_chars_in_string(s, '\t') * 8) +
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
  for (size_t i = 0; i < strlen_s; i++) {
    if (has_query) {
      if (beg == i) {
        strcat(rv, COLOR_RED);
        cursor += strlen(COLOR_RED);
      } else if (end == i) {
        strcat(rv, COLOR_DEFAULT);
        cursor += strlen(COLOR_DEFAULT);
      }
    }
    if (s[i] == '\t') {
      do {
        rv[cursor++] = ' ';
        visible_chars++;
      } while (visible_chars % 8 != 0);
    } else {
      rv[cursor++] = s[i];
      visible_chars++;
    }
  }
  if (selected) {
    strcat(rv, COLOR_RESET);
  }
  strcat(rv, CLEAR_LINE "\r\n");
  return rv;
}

Renderer *renderer_new() {
  Renderer *r = malloc(sizeof(Renderer));
  return r;
}

char *renderer_render(Renderer *r) {
  char *rv = calloc(16384, sizeof(char)); // adjust this later?
  strcat(rv, HIDE_CURSOR "\r\n");
  Score *score;
  const bool has_query = strlen(r->query) > 0;
  for (size_t i = 0; i < r->height - 1; i++) {
    if (r->scores->length > i) {
      score = r->scores->items[i];
      char *line = render_line(score->line, score->first, score->last,
                               r->selected == i, has_query);
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
  size_t length =
      strlen(r->query) + strlen(" > " SHOW_CURSOR CLEAR_LINE) + r->match_length;
  char prompt[length];
  memset(prompt, 0, length);
  sprintf(prompt, "%*zd > %s%s%s", r->match_length, r->scores->length, r->query,
          SHOW_CURSOR, CLEAR_LINE);
  strcat(rv, prompt);
  return rv;
}
