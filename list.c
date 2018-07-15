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

List *list_new() { return list_new_of_size(10); }

List *list_new_of_size(size_t size) {
  List *list = malloc(sizeof(List));
  list->length = 0;
  list->capacity = size;
  list->size = size;
  list->items = calloc(list->capacity, sizeof(void *));
  return list;
}

void list_free(List *list) {
  if (list == NULL) {
    return;
  }
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
  qsort(list->items, list->length, sizeof(list->items[0]), cmp);
}

static int _list_sort_by_score_cmp(const void *_a, const void *_b) {
  if (_a == NULL || _b == NULL) {
    return 0;
  }
  Score *a = *(Score **)_a;
  Score *b = *(Score **)_b;
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

// static void *strtoupper(void *_s) {
//   char *s = (char *)_s;
//   char *rv = calloc(strlen(s), sizeof(char));
//   for (size_t i = 0; i < strlen(s); i++) {
//     rv[i] = toupper(s[i]);
//   }
//   return (char *)rv;
// }

// static bool strcontains(const char *s, char c) {
//   return find_char_idx(s, c) > -1;
// }

// static bool x(const void *s) { return strcontains((char *)s, 'x'); }

// int main(int argc, char *argv[]) {
//   List *list = list_new();
//   list_push(list, "one");
//   list_push(list, "x two");
//   list_push(list, "three");
//   list_push(list, "x four");

//   List *uppers = list_map(list, strtoupper);
//   List *has_xes = list_filter(list, x);

//   list_log(list);
//   list_log(uppers);
//   list_log(has_xes);
//   printf("list_pop list %s\n", list_pop(list));
//   printf("list_pop list %s\n", list_pop(list));
//   printf("list_pop list %s\n", list_pop(list));
//   list_log(list);

//   return EXIT_SUCCESS;
// }
