#include "test.h"
#include "util.h"
#include <string.h>

void test_strtolower() {
  test_assert(strcmp(strtolower("FOOBAR"), "foobar") == 0);
  test_assert(strcmp(strtolower("BAZQUUX"), "BAZQUUX") != 0);
}

void test_find_char_idx() {
  test_assert(find_char_idx("foobar", 'b') == 3);
  test_assert(find_char_idx("foobar", 'z') == -1);
}
