#include "render.h"
#include "test.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void test_highlight_line() {
  char expected1[] = COLOR_RESET "foo" COLOR_RED "bar" COLOR_RESET "baz";
  char *result1 = highlight_line("foobarbaz", 3, 6, false);
  assert(strcmp(expected1, result1) == 0);
  free(result1);
  char expected2[] =
      COLOR_RESET COLOR_REVERSE "foo" COLOR_RED "bar" COLOR_RESET "baz";
  char *result2 = highlight_line("foobarbaz", 3, 6, true);
  assert(strcmp(expected2, result2) == 0);
  free(result2);
}

void test_expand_tabs() {
  char expected1[] = "foo    bar";
  char *result1 = expand_tabs("foo	bar");
  assert(strcmp(expected1, result1));
  free(result1);
  char expected2[] = "bazquux        garply";
  char *result2 = expand_tabs("bazquux	garply");
  assert(strcmp(expected2, result2));
  free(result2);
  char expected3[] = "foo    bar     baz     quux            garply";
  char *result3 = expand_tabs("foo	bar	baz	quux		"
                              "garply");
  assert(strcmp(expected3, result3));
  free(result3);
}
