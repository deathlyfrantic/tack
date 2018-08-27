#include "render.h"
#include "score.h"
#include "util.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void render_line(Renderer *renderer, Score *score, const bool selected,
                        String *output) {
  // this function:
  // - highlights match
  // - expands tabs
  // - truncates line to width
  string_concat(output, COLOR_RESET);
  size_t visible_chars = 0;
  if (selected) {
    string_concat(output, COLOR_REVERSE);
  }
  for (size_t i = 0; i < score->line->length; i++) {
    if (score->first != score->last) {
      if (score->first == i) {
        string_concat(output, COLOR_RED);
      } else if (score->last == i) {
        string_concat(output, COLOR_DEFAULT);
      }
    }
    if (score->line->buf[i] == '\t') {
      // expand tabs into spaces (actual tabs break reversed highlighting)
      do {
        string_push_char(output, ' ');
        visible_chars++;
      } while (renderer->width > visible_chars && visible_chars % 8 != 0);
    } else if (renderer->width > visible_chars) {
      string_push_char(output, score->line->buf[i]);
      visible_chars++;
    }
    if (renderer->width <= visible_chars) {
      // truncate at width of screen
      break;
    }
  }
  string_concat(output, COLOR_RESET CLEAR_LINE "\r\n");
}

String *renderer_render(Renderer *r) {
  String *output = string_new_without_low_from(HIDE_CURSOR "\r\n");
  Score *score;
  for (size_t i = 0; i < r->height - 1; i++) {
    if (r->scores->length > i) {
      score = r->scores->items[i];
      render_line(r, score, r->selected == i, output);
    } else {
      string_concat(output, CLEAR_LINE "\r\n");
    }
  }
  char clear[10] = "";
  memset(clear, 0, 10);
  sprintf(clear, "\e[%" PRIu16 "A", r->height);
  string_concat(output, clear);
  char prompt[r->width];
  memset(prompt, 0, r->width);
  snprintf(prompt, r->width, "%*zd > %s", r->match_length, r->scores->length,
           r->query);
  string_concat(output, prompt);
  string_concat(output, SHOW_CURSOR CLEAR_LINE);
  return output;
}

#ifdef TESTS
#include "test.h"

void test_render_line() {
  Renderer r = { .width = 200 };
  String *result = string_new_without_low();
  String *line = string_new_from("foo\tbarbaz");
  Score *s = score_new();
  s->first = 4;
  s->last = 7;
  s->line = line;
  // test expand tabs, highlighting
  char expected1[] = COLOR_RESET "foo     " COLOR_RED "bar" COLOR_DEFAULT
                                 "baz" CLEAR_LINE "\r\n";
  render_line(&r, s, false, result);
  test_assert(string_equals(result, expected1));
  string_reset(result);
  // test highlighting, reverse for selected line
  char expected2[] = COLOR_RESET COLOR_REVERSE
    "foo" COLOR_RED "bar" COLOR_DEFAULT "baz" COLOR_RESET CLEAR_LINE "\r\n";
  s->first = 3;
  s->last = 6;
  string_set(line, "foobarbaz");
  render_line(&r, s, true, result);
  test_assert(string_equals(result, expected2));
  string_reset(result);
  // test expand tabs, highlighting
  char expected3[] =
    COLOR_RESET "foo     bar     baz     " COLOR_RED
                "quux            gar" COLOR_DEFAULT "ply" CLEAR_LINE "\r\n";
  string_set(line, "foo\tbar\tbaz\tquux\t\tgarply");
  String *query = string_new_from("qr");
  Score *score = calculate_score(line, query);
  render_line(&r, score, false, result);
  test_assert(string_equals(result, expected3));
  free(score);
  string_free(query);
  string_reset(result);
  // test no highlighting when no query
  s->first = 0;
  s->last = 0;
  string_set(line, "foobarbaz");
  char expected4[] = COLOR_RESET "foobarbaz" CLEAR_LINE "\r\n";
  render_line(&r, s, false, result);
  test_assert(string_equals(result, expected4));
  string_reset(result);
  // test expand tabs by itself
  string_set(line, "foo\tbar");
  char expected5[] = COLOR_RESET "foo     bar" CLEAR_LINE "\r\n";
  render_line(&r, s, false, result);
  test_assert(string_equals(result, expected5));
  string_reset(result);
  // test expand tabs when tab is only one space wide visibly
  string_set(line, "bazquux\tgarply");
  char expected6[] = COLOR_RESET "bazquux garply" CLEAR_LINE "\r\n";
  render_line(&r, s, false, result);
  test_assert(string_equals(result, expected6));
  string_reset(result);
  // test line truncation with highlighting
  string_set(line, "foobarbazquuxgarply");
  r.width = 8;
  s->first = 3;
  s->last = 6;
  char expected7[] =
    COLOR_RESET "foo" COLOR_RED "bar" COLOR_DEFAULT "ba" CLEAR_LINE "\r\n";
  render_line(&r, s, false, result);
  test_assert(string_equals(result, expected7));
  string_free(result);
  string_free(line);
  free(s);
}

#endif
