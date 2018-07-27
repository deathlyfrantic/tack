#include "score.h"
#include "test.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

void test_score_new() {
  Score *score = score_new();
  test_assert(score->points == UINT_MAX);
  test_assert(score->first == 0);
  test_assert(score->last == 0);
  test_assert(score->line == NULL);
}

void test_calculate_score() {
  // verify NULL is returned if line doesn't contain query chars
  test_assert(calculate_score("foo", "qxz") == NULL);
  Score *a = calculate_score("oof", "f");
  test_assert(a->points == 1);
  test_assert(a->first == 2);
  test_assert(a->last == 3);
  test_assert(strcmp(a->line, "oof") == 0); // just to verify line gets set
  free(a);
  // this should be case insensitive
  Score *b = calculate_score("oof", "F");
  test_assert(b != NULL);
  test_assert(b->points == 1);
  test_assert(b->first == 2);
  test_assert(b->last == 3);
  free(b);
  Score *c = calculate_score("foofbbar", "fob");
  test_assert(c->points == 5);
  test_assert(c->first == 0);
  test_assert(c->last == 5);
  free(c);
  Score *d = calculate_score("foo / ba r", "or");
  test_assert(d->points == 2);
  test_assert(d->first == 1);
  test_assert(d->last == 10);
  free(d);
  Score *e = calculate_score("f||||||||b||||||||||||||a||||f||||||||r", "bar");
  test_assert(e->points == 2);
  test_assert(e->first == 9);
  test_assert(e->last == 39);
  free(e);
}
