#ifndef TACK_LIST_H
#define TACK_LIST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  size_t length;
  size_t capacity;
  void **items;
  size_t size;
} List;

List *list_new();
List *list_new_of_size(size_t);
void list_free(List *);
void list_push(List *, void *);
void *list_pop(List *);
List *list_filter(List *, bool (*)(const void *));
List *list_map(List *, void *(*)(void *));
void list_sort(List *, int (*)(const void *, const void *));
void list_sort_by_score(List *);
void _list_log(const char *, List *);

#define list_log(a) _list_log(#a, a);

#endif /* end of include guard: TACK_LIST_H */
