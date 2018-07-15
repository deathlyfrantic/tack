#include "test.h"
#include "util.h"
#include <string.h>

void test_strtolower() {
  assert(strcmp(strtolower("FOOBAR"), "foobar") == 0);
  assert(strcmp(strtolower("BAZQUUX"), "BAZQUUX") != 0);
}

void test_find_char_idx() {
  assert(find_char_idx("foobar", 'b') == 3);
  assert(find_char_idx("foobar", 'z') == -1);
}
