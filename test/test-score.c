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
  Score *score1 = calculate_score("oof", "f");
  test_assert(score1->points == 1);
  test_assert(score1->first == 2);
  test_assert(score1->last == 3);
  test_assert(strcmp(score1->line, "oof") == 0); // just to verify line gets set
  free(score1);
  // this should be case insensitive
  Score *score2 = calculate_score("oof", "F");
  test_assert(score2 != NULL);
  test_assert(score2->points == 1);
  test_assert(score2->first == 2);
  test_assert(score2->last == 3);
  free(score2);
  Score *score3 = calculate_score("foofbbar", "fob");
  test_assert(score3->points == 5);
  test_assert(score3->first == 0);
  test_assert(score3->last == 5);
  free(score3);
  Score *score4 = calculate_score("foo / ba r", "or");
  test_assert(score4->points == 2);
  test_assert(score4->first == 1);
  test_assert(score4->last == 10);
  free(score4);
  Score *score5 =
      calculate_score("f||||||||b||||||||||||||a||||f||||||||r", "bar");
  test_assert(score5->points == 2);
  test_assert(score5->first == 9);
  test_assert(score5->last == 39);
  free(score5);
}
