#include "list.h"
#include "score.h"
#include "util.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void list_grow(List *list) {
  list->capacity += list->size;
  list->items = realloc(list->items, sizeof(void *) * list->capacity);
}

List *list_new() {
  return list_new_of_size(10);
}

List *list_new_of_size(size_t size) {
  List *list = malloc(sizeof(List));
  list->length = 0;
  list->capacity = size;
  list->size = size;
  list->items = calloc(list->capacity, sizeof(void *));
  return list;
}

void list_free(List *list) {
  if (list == NULL) return;
  free(list->items);
  free(list);
}

void list_push(List *list, void *item) {
  if (list->length == list->capacity) {
    list_grow(list);
  }
  list->items[list->length] = item;
  list->length++;
}

void *list_pop(List *list) {
  if (list->length == 0) {
    return NULL;
  }
  list->length--;
  void *tmp = list->items[list->length];
  list->items[list->length] = NULL;
  return tmp;
}

List *list_filter(List *list, bool (*f)(const void *)) {
  List *rv = list_new_of_size(list->size);
  for (size_t i = 0; i < list->length; i++) {
    if (f(list->items[i])) {
      list_push(rv, list->items[i]);
    }
  }
  return rv;
}

List *list_map(List *list, void *(*f)(void *)) {
  List *rv = list_new_of_size(list->size);
  for (size_t i = 0; i < list->length; i++) {
    list_push(rv, f(list->items[i]));
  }
  return rv;
}

void list_sort(List *list, int (*cmp)(const void *, const void *)) {
  if (list->length < 2) {
    return;
  }
  mergesort(list->items, list->length, sizeof(list->items[0]), cmp);
}

static int _list_sort_by_score_cmp(const void *_a, const void *_b) {
  if (_a == NULL || _b == NULL) {
    return 0;
  }
  const Score *a = *(Score **)_a;
  const Score *b = *(Score **)_b;
  return a->points == b->points ? 0 : a->points > b->points ? 1 : -1;
}

void list_sort_by_score(List *list) {
  list_sort(list, _list_sort_by_score_cmp);
}

void _list_log(const char *name, List *list) {
  printf("[List \"%s\" (%p)]", name, list);
  if (list != NULL) {
    printf(" length: %zu; capacity: %zu; _size: %zu", list->length,
           list->capacity, list->size);
    for (size_t i = 0; i < list->length; i++) {
      printf("  item %zu: %s\n", i, list->items[i]);
    }
  }
  printf("\n");
}

#ifdef TESTS
#include "test.h"

void test_list_new_of_size() {
  List *list = list_new_of_size(27);
  test_assert(list->length == 0);
  test_assert(list->capacity == 27);
  test_assert(list->size == 27);
}

void test_list_new() {
  List *list = list_new();
  test_assert(list->size == 10);
}

void test_list_grow() {
  List *list = list_new_of_size(1);
  list_push(list, "foo");
  test_assert(list->length == 1);
  test_assert(list->capacity == 1);
  list_push(list, "bar");
  test_assert(list->length == 2);
  test_assert(list->capacity == 2);
}

void test_list_push() {
  List *list = list_new();
  test_assert(list->length == 0);
  list_push(list, "foo");
  test_assert(list->length == 1);
  test_assert(strcmp(list->items[0], "foo") == 0);
}

void test_list_pop() {
  List *list = list_new();
  test_assert(list->length == 0);
  list_push(list, "foo");
  list_push(list, "bar");
  test_assert(list->length == 2);
  const char *item = list_pop(list);
  test_assert(strcmp(item, "bar") == 0);
  test_assert(list->length == 1);
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
  test_assert(*(int *)list->items[0] == 3);
  test_assert(*(int *)list->items[1] == 4);
  test_assert(*(int *)list->items[2] == 2);
  test_assert(*(int *)list->items[3] == 1);
  test_assert(*(int *)list->items[4] == 5);
  list_sort(list, test_list_sort_cmp);
  test_assert(*(int *)list->items[0] == 1);
  test_assert(*(int *)list->items[1] == 2);
  test_assert(*(int *)list->items[2] == 3);
  test_assert(*(int *)list->items[3] == 4);
  test_assert(*(int *)list->items[4] == 5);
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
  test_assert(((Score *)list->items[0])->points == 20);
  test_assert(((Score *)list->items[1])->points == 30);
  test_assert(((Score *)list->items[2])->points == 10);
  list_sort_by_score(list);
  test_assert(((Score *)list->items[0])->points == 10);
  test_assert(((Score *)list->items[1])->points == 20);
  test_assert(((Score *)list->items[2])->points == 30);
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
  test_assert(uppers->length == 4);
  test_assert(strcmp(uppers->items[0], "FOO") == 0);
  test_assert(strcmp(uppers->items[1], "BAR") == 0);
  test_assert(strcmp(uppers->items[2], "BAZ") == 0);
  test_assert(strcmp(uppers->items[3], "QUUX") == 0);
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
  test_assert(evens->length == 3);
  test_assert(*(int *)evens->items[0] == 2);
  test_assert(*(int *)evens->items[1] == 4);
  test_assert(*(int *)evens->items[2] == 6);
}

#endif
