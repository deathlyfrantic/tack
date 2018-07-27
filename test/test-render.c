#include "list.h"
#include "render.h"
#include "score.h"
#include "test.h"
#include "util.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void test_render_line() {
  char expected1[] = COLOR_RESET "foo     " COLOR_RED "bar" COLOR_DEFAULT
                                 "baz" CLEAR_LINE "\r\n";
  char *result1 = render_line("foo\tbarbaz", 4, 7, false, true, 200);
  test_assert(strcmp(expected1, result1) == 0);
  free(result1);
  char expected2[] = COLOR_RESET COLOR_REVERSE
      "foo" COLOR_RED "bar" COLOR_DEFAULT "baz" COLOR_RESET CLEAR_LINE "\r\n";
  char *result2 = render_line("foobarbaz", 3, 6, true, true, 200);
  test_assert(strcmp(expected2, result2) == 0);
  free(result2);
  char expected3[] =
      COLOR_RESET "foo     bar     baz     " COLOR_RED
                  "quux            gar" COLOR_DEFAULT "ply" CLEAR_LINE "\r\n";
  Score *score = calculate_score("foo\tbar\tbaz\tquux\t\tgarply", "qr");
  char *result3 =
      render_line(score->line, score->first, score->last, false, true, 200);
  test_assert(strcmp(expected3, result3) == 0);
  free(score);
  free(result3);
  char expected4[] =
      COLOR_RESET "foo" COLOR_RED "bar" COLOR_DEFAULT "baz" CLEAR_LINE "\r\n";
  char *result4 = render_line("foobarbaz", 3, 6, false, true, 200);
  test_assert(strcmp(expected4, result4) == 0);
  free(result4);
  char expected5[] = COLOR_RESET COLOR_REVERSE
      "foo" COLOR_RED "bar" COLOR_DEFAULT "baz" COLOR_RESET CLEAR_LINE "\r\n";
  char *result5 = render_line("foobarbaz", 3, 6, true, true, 200);
  test_assert(strcmp(expected5, result5) == 0);
  free(result5);
  char expected6[] = COLOR_RESET "foobarbaz" CLEAR_LINE "\r\n";
  char *result6 = render_line("foobarbaz", 0, 0, false, false, 200);
  test_assert(strcmp(expected6, result6) == 0);
  free(result6);
  char expected7[] = COLOR_RESET "foo     bar" CLEAR_LINE "\r\n";
  char *result7 = render_line("foo\tbar", 0, 0, false, false, 200);
  test_assert(strcmp(expected7, result7) == 0);
  free(result7);
  char expected8[] = COLOR_RESET "bazquux garply" CLEAR_LINE "\r\n";
  char *result8 = render_line("bazquux\tgarply", 0, 0, false, false, 200);
  test_assert(strcmp(expected8, result8) == 0);
  free(result8);
  char expected9[] =
      COLOR_RESET "foo" COLOR_RED "bar" COLOR_DEFAULT "ba" CLEAR_LINE "\r\n";
  char *result9 = render_line("foobarbazquuxgarply", 3, 6, false, true, 8);
  test_assert(strcmp(expected9, result9) == 0);
  free(result9);
}
