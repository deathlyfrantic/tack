#include "render.h"
#include "score.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *highlight_line(const char *s, size_t beg, size_t end, bool selected,
                     bool has_query) {
  size_t length = strlen(s) +
                  strlen(COLOR_RESET COLOR_REVERSE COLOR_RED COLOR_DEFAULT
                             COLOR_RESET CLEAR_LINE) +
                  3; // + 2 for \r\n, + 1 for \0
  char *rv = calloc(length, sizeof(char));
  size_t cursor = strlen(COLOR_RESET);
  sprintf(rv, "%s", COLOR_RESET);
  if (selected) {
    for (size_t i = 0; i < strlen(COLOR_REVERSE); i++) {
      rv[cursor++] = COLOR_REVERSE[i];
    }
  }
  if (has_query) {
    for (size_t i = 0; i < strlen(s); i++) {
      if (beg == i) {
        for (size_t j = 0; j < strlen(COLOR_RED); j++) {
          rv[cursor++] = COLOR_RED[j];
        }
      } else if (end == i) {
        for (size_t j = 0; j < strlen(COLOR_DEFAULT); j++) {
          rv[cursor++] = COLOR_DEFAULT[j];
        }
      }
      rv[cursor++] = s[i];
    }
  } else {
    strcat(rv, s);
  }
  if (selected) {
    strcat(rv, COLOR_RESET);
  }
  strcat(rv, CLEAR_LINE "\r\n");
  return rv;
}

char *expand_tabs(const char *s) {
  size_t length = 1; // account for \0
  for (size_t i = 0; i < strlen(s); i++) {
    length += (s[i] == '\t') ? 8 : 1;
  }
  char *rv = calloc(length, sizeof(char));
  size_t cursor = 0;
  for (size_t i = 0; i < strlen(s); i++) {
    if (s[i] == '\t') {
      do {
        rv[cursor++] = ' ';
      } while (cursor % 8 != 0);
    } else {
      rv[cursor++] = s[i];
    }
  }
  rv[cursor] = '\0';
  return rv;
}

Renderer *renderer_new() {
  Renderer *r = malloc(sizeof(Renderer));
  return r;
}

char *renderer_render(Renderer *r) {
  char *rv = calloc(16384, sizeof(char)); // adjust this later?
  sprintf(rv, "%s\r\n", HIDE_CURSOR);
  Score *score;
  const bool has_query = strlen(r->query) > 0;
  for (size_t i = 0; i < r->height - 1; i++) {
    if (r->scores->length > i) {
      score = r->scores->items[i];
      char *expanded = expand_tabs(score->line);
      char *line = highlight_line(expanded, score->first, score->last,
                                  r->selected == i, has_query);
      strcat(rv, line);
      free(line);
      free(expanded);
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
