#include "render.h"
#include "score.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *render_line(Renderer *renderer, Score *score,
                         const bool selected) {
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

#ifdef TESTS
#include "test.h"

void test_render_line() {
  Renderer *r = renderer_new();
  r->width = 200;
  Score *s = score_new();
  s->first = 4;
  s->last = 7;
  s->line = "foo\tbarbaz";
  // test expand tabs, highlighting
  char expected1[] = COLOR_RESET "foo     " COLOR_RED "bar" COLOR_DEFAULT
                                 "baz" CLEAR_LINE "\r\n";
  char *result1 = render_line(r, s, false);
  test_assert(strcmp(expected1, result1) == 0);
  free(result1);
  // test highlighting, reverse for selected line
  char expected2[] = COLOR_RESET COLOR_REVERSE
    "foo" COLOR_RED "bar" COLOR_DEFAULT "baz" COLOR_RESET CLEAR_LINE "\r\n";
  s->first = 3;
  s->last = 6;
  s->line = "foobarbaz";
  char *result2 = render_line(r, s, true);
  test_assert(strcmp(expected2, result2) == 0);
  free(result2);
  // test expand tabs, highlighting
  char expected3[] =
    COLOR_RESET "foo     bar     baz     " COLOR_RED
                "quux            gar" COLOR_DEFAULT "ply" CLEAR_LINE "\r\n";
  Score *score = calculate_score("foo\tbar\tbaz\tquux\t\tgarply", "qr");
  char *result3 = render_line(r, score, false);
  test_assert(strcmp(expected3, result3) == 0);
  free(score);
  free(result3);
  // test no highlighting when no query
  s->first = 0;
  s->last = 0;
  char expected4[] = COLOR_RESET "foobarbaz" CLEAR_LINE "\r\n";
  char *result4 = render_line(r, s, false);
  test_assert(strcmp(expected4, result4) == 0);
  free(result4);
  // test expand tabs by itself
  s->line = "foo\tbar";
  char expected5[] = COLOR_RESET "foo     bar" CLEAR_LINE "\r\n";
  char *result5 = render_line(r, s, false);
  test_assert(strcmp(expected5, result5) == 0);
  free(result5);
  // test expand tabs when tab is only one space wide visibly
  s->line = "bazquux\tgarply";
  char expected6[] = COLOR_RESET "bazquux garply" CLEAR_LINE "\r\n";
  char *result6 = render_line(r, s, false);
  test_assert(strcmp(expected6, result6) == 0);
  free(result6);
  // test line truncation with highlighting
  s->line = "foobarbazquuxgarply";
  r->width = 8;
  s->first = 3;
  s->last = 6;
  char expected7[] =
    COLOR_RESET "foo" COLOR_RED "bar" COLOR_DEFAULT "ba" CLEAR_LINE "\r\n";
  char *result7 = render_line(r, s, false);
  test_assert(strcmp(expected7, result7) == 0);
  free(result7);
}

#endif
