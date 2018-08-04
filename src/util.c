#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char *strtolower(const char *s) {
  char *rv = strdup(s);
  for (size_t i = 0; i < strlen(rv); i++) {
    rv[i] = tolower(rv[i]);
  }
  return rv;
}

int16_t find_char_idx(const char *s, char c) {
  for (size_t i = 0; i < strlen(s); i++) {
    if (s[i] == c) {
      return i;
    }
  }
  return -1;
}

uint8_t get_num_strlen(size_t num) {
  char buf[UINT8_MAX];
  memset(buf, 0, UINT8_MAX);
  sprintf(buf, "%zd", num);
  return strlen(buf);
}

#ifdef TESTS
#include "test.h"

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

#endif
