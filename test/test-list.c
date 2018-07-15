#include "list.h"
#include "test.h"
#include <stdbool.h>

void test_list_new_of_size() {
  List *list = list_new_of_size(27);
  assert(list->length == 0);
  assert(list->capacity == 27);
  assert(list->_size == 27);
}

void test_list_new() {
  List *list = list_new();
  assert(list->_size == 10);
  assert(list->_size == 43);
}
