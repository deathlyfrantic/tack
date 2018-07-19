#include "render.h"
#include "score.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *highlight_line(const char *s, size_t beg, size_t end, bool selected) {
  size_t length = strlen(s) +
                  strlen(COLOR_RESET COLOR_REVERSE COLOR_RED COLOR_DEFAULT) +
                  2; // + 1 for \n, + 1 for \0
  char *rv = malloc(sizeof(char) * length);
  size_t cursor = strlen(COLOR_RESET);
  sprintf(rv, "%s", COLOR_RESET);
  if (selected) {
    for (size_t i = 0; i < strlen(COLOR_REVERSE); i++) {
      rv[cursor++] = COLOR_REVERSE[i];
    }
  }
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
  rv[cursor++] = '\n';
  rv[cursor] = '\0';
  return rv;
}

char *expand_tabs(const char *s) {
  size_t length = 1; // account for \0
  for (size_t i = 0; i < strlen(s); i++) {
    length += (s[i] == '\t') ? 8 : 1;
  }
  char *rv = malloc(sizeof(char) * length);
  size_t cursor = 0;
  for (size_t i = 0; i < strlen(s); i++) {
    if (s[i] == '\t') {
      while (cursor % 8 != 0) {
        rv[cursor++] = ' ';
      }
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
  char *rv = malloc(sizeof(char) * 16384); // adjust this later?
  sprintf(rv, "%*zd > %s\n", r->match_length, r->scores->length, r->query);
  Score *score;
  for (size_t i = 0; i < r->height - 1 && i < r->scores->length; i++) {
    score = r->scores->items[i];
    char *expanded = expand_tabs(score->line);
    char *line =
        highlight_line(expanded, score->first, score->last, r->selected == i);
    strcat(rv, line);
    free(expanded);
    free(line);
  }
  return rv;
}
