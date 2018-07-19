#include "list.h"
#include "render.h"
#include "score.h"
#include "test.h"
#include "util.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void test_highlight_line() {
  char expected1[] = COLOR_RESET "foo" COLOR_RED "bar" COLOR_DEFAULT "baz\n";
  char *result1 = highlight_line("foobarbaz", 3, 6, false);
  test_assert(strcmp(expected1, result1) == 0);
  free(result1);
  char expected2[] =
      COLOR_RESET COLOR_REVERSE "foo" COLOR_RED "bar" COLOR_DEFAULT "baz\n";
  char *result2 = highlight_line("foobarbaz", 3, 6, true);
  test_assert(strcmp(expected2, result2) == 0);
  free(result2);
}

void test_expand_tabs() {
  char expected1[] = "foo    bar";
  char *result1 = expand_tabs("foo\tbar");
  test_assert(strcmp(expected1, result1));
  free(result1);
  char expected2[] = "bazquux        garply";
  char *result2 = expand_tabs("bazquux\tgarply");
  test_assert(strcmp(expected2, result2));
  free(result2);
  char expected3[] = "foo    bar     baz     quux            garply";
  char *result3 = expand_tabs("foo\tbar\tbaz\tquux\t\tgarply");
  test_assert(strcmp(expected3, result3));
  free(result3);
}

void test_renderer_render() {
  char query[] = "fo";
  Score *a = calculate_score("floof", query);
  Score *b = calculate_score("ffbboar", query);
  Score *c = calculate_score("fbarolyo", query);
  List *scores = list_new_of_size(3);
  list_push(scores, a);
  list_push(scores, b);
  list_push(scores, c);
  Renderer *r = renderer_new();
  r->scores = scores;
  r->query = query;
  r->height = 15;
  r->selected = 1;
  r->match_length = get_num_strlen(scores->length);
  char *output = renderer_render(r);
  puts(output);
  free(output);
  free(a);
  free(b);
  free(c);
  list_free(scores);
  free(r);
}
