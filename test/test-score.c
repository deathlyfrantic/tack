#include "score.h"
#include "test.h"
#include <string.h>

#include <limits.h>

void test_score_new() {
  Score *score = score_new();
  assert(score->points == UINT_MAX);
  assert(score->first == 0);
  assert(score->last == 0);
  assert(score->line == NULL);
}

void test_calculate_score() {
  // verify NULL is returned if line doesn't contain query chars
  assert(calculate_score("foo", "qxz") == NULL);

  Score *a = calculate_score("oof", "f");
  assert(a->points == 1);
  assert(a->first == 2);
  assert(a->last == 2);
  assert(strcmp(a->line, "oof") == 0); // just to verify line gets set

  // this should be case insensitive
  Score *b = calculate_score("oof", "F");
  assert(b != NULL);
  assert(b->points == 1);
  assert(b->first == 2);
  assert(b->last == 2);

  Score *c = calculate_score("foofbbar", "fob");
  assert(c->points == 5);
  assert(c->first == 0);
  assert(c->last == 4);

  Score *d = calculate_score("foo / ba r", "or");
  assert(d->points == 2);
  assert(d->first == 1);
  assert(d->last == 9);

  Score *e = calculate_score("f||||||||b||||||||||||||a||||f||||||||r", "bar");
  assert(e->points == 2);
  assert(e->first == 9);
  assert(e->last == 38);
}
