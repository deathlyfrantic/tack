#include "list.h"
#include "render.h"
#include "score.h"
#include "test.h"
#include "util.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void test_render_line() {
  Renderer *r = renderer_new();
  r->width = 200;
  r->has_query = true;
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
  r->has_query = false;
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
  r->has_query = true;
  char expected7[] =
      COLOR_RESET "foo" COLOR_RED "bar" COLOR_DEFAULT "ba" CLEAR_LINE "\r\n";
  char *result7 = render_line(r, s, false);
  test_assert(strcmp(expected7, result7) == 0);
  free(result7);
}
