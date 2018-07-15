#include "list.h"
#include "score.h"
#include "test.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void test_list_new_of_size() {
  List *list = list_new_of_size(27);
  assert(list->length == 0);
  assert(list->capacity == 27);
  assert(list->size == 27);
}

void test_list_new() {
  List *list = list_new();
  assert(list->size == 10);
}

void test_list_grow() {
  List *list = list_new_of_size(1);
  list_push(list, "foo");
  assert(list->length == 1);
  assert(list->capacity == 1);
  list_push(list, "bar");
  assert(list->length == 2);
  assert(list->capacity == 2);
}

void test_list_push() {
  List *list = list_new();
  assert(list->length == 0);
  list_push(list, "foo");
  assert(list->length == 1);
  assert(strcmp(list->items[0], "foo") == 0);
}

void test_list_pop() {
  List *list = list_new();
  assert(list->length == 0);
  list_push(list, "foo");
  list_push(list, "bar");
  assert(list->length == 2);
  const char *item = list_pop(list);
  assert(strcmp(item, "bar") == 0);
  assert(list->length == 1);
}

static int test_list_sort_cmp(const void *_a, const void *_b) {
  int a = *(int *)_a;
  int b = *(int *)_b;
  return a == b ? 0 : a < b ? 1 : -1;
}

void test_list_sort() {
  List *list = list_new();
  int a = 1, b = 2, c = 3, d = 4, e = 5;
  list_push(list, &c);
  list_push(list, &d);
  list_push(list, &b);
  list_push(list, &a);
  list_push(list, &e);
  assert(*(int *)list->items[0] == 3);
  assert(*(int *)list->items[1] == 4);
  assert(*(int *)list->items[2] == 2);
  assert(*(int *)list->items[3] == 1);
  assert(*(int *)list->items[4] == 5);
  list_sort(list, test_list_sort_cmp);
  assert(*(int *)list->items[0] == 1);
  assert(*(int *)list->items[1] == 2);
  assert(*(int *)list->items[2] == 3);
  assert(*(int *)list->items[3] == 4);
  assert(*(int *)list->items[4] == 5);
}

void test_list_sort_by_score() {
  List *list = list_new();
  Score *score1 = score_new();
  Score *score2 = score_new();
  Score *score3 = score_new();
  score1->points = 20;
  score2->points = 30;
  score3->points = 10;
  list_push(list, score1);
  list_push(list, score2);
  list_push(list, score3);
  assert(((Score *)list->items[0])->points == 20);
  assert(((Score *)list->items[1])->points == 30);
  assert(((Score *)list->items[2])->points == 10);
  list_sort_by_score(list);
  assert(((Score *)list->items[0])->points == 10);
  assert(((Score *)list->items[1])->points == 20);
  assert(((Score *)list->items[2])->points == 30);
  free(score1);
  free(score2);
  free(score3);
}

static void *test_list_map_to_upper(void *_s) {
  char *s = (char *)_s;
  char *rv = calloc(strlen(s), sizeof(char));
  for (size_t i = 0; i < strlen(s); i++) {
    rv[i] = toupper(s[i]);
  }
  return (char *)rv;
}

void test_list_map() {
  List *list = list_new();
  list_push(list, "foo");
  list_push(list, "bar");
  list_push(list, "baz");
  list_push(list, "quux");
  List *uppers = list_map(list, test_list_map_to_upper);
  assert(uppers->length == 4);
  assert(strcmp(uppers->items[0], "FOO") == 0);
  assert(strcmp(uppers->items[1], "BAR") == 0);
  assert(strcmp(uppers->items[2], "BAZ") == 0);
  assert(strcmp(uppers->items[3], "QUUX") == 0);
}

static bool test_list_filter_filterer(const void *x) {
  return *(int *)x % 2 == 0;
}

void test_list_filter() {
  List *list = list_new();
  int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;
  list_push(list, &a);
  list_push(list, &b);
  list_push(list, &c);
  list_push(list, &d);
  list_push(list, &e);
  list_push(list, &f);
  List *evens = list_filter(list, test_list_filter_filterer);
  assert(evens->length == 3);
  assert(*(int *)evens->items[0] == 2);
  assert(*(int *)evens->items[1] == 4);
  assert(*(int *)evens->items[2] == 6);
}
