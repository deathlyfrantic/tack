#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t get_num_strlen(size_t num) {
  char buf[UINT8_MAX];
  memset(buf, 0, UINT8_MAX);
  sprintf(buf, "%zd", num);
  return strlen(buf);
}

#ifdef TESTS
#include "test.h"

void test_get_num_strlen() {
  test_assert(get_num_strlen(5) == 1);
  test_assert(get_num_strlen(50) == 2);
  test_assert(get_num_strlen(500) == 3);
  test_assert(get_num_strlen(5000) == 4);
  test_assert(get_num_strlen(50000) == 5);
}

#endif
