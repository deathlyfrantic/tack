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

void test_get_num_strlen() {
  test_assert(get_num_strlen(5) == 1);
  test_assert(get_num_strlen(50) == 2);
  test_assert(get_num_strlen(500) == 3);
  test_assert(get_num_strlen(5000) == 4);
  test_assert(get_num_strlen(50000) == 5);
}
