#include "score.h"
#include "util.h"
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct match {
  uint16_t score;
  uint16_t last;
};

enum match_type {
  MATCH_TYPE_SEQUENTIAL,
  MATCH_TYPE_BOUNDARY,
  MATCH_TYPE_NORMAL,
};

Score *score_new() {
  Score *score = malloc(sizeof(Score));
  score->points = UINT16_MAX;
  score->first = 0;
  score->last = 0;
  score->line = NULL;
  return score;
}

static bool single_char_query(Score *score, String *query) {
  int32_t idx = string_find_ichar(score->line, string_get_char_at(query, 0));
  if (idx == -1) return false;
  score->first = idx;
  score->last = idx + 1;
  score->points = 1;
  return true;
}

static bool find_end_of_match(struct match *m, String *line, String *query,
                              size_t start) {
  uint16_t last_index = start, score = 1;
  int32_t index;
  enum match_type last_match_type = MATCH_TYPE_NORMAL;
  for (size_t i = 1; i < query->length; i++) {
    index =
      string_find_ichar_from(line, string_get_char_at(query, i), last_index);
    if (index == -1) return false;
    index += last_index;
    if (index == last_index + 1) {
      if (last_match_type != MATCH_TYPE_SEQUENTIAL) {
        last_match_type = MATCH_TYPE_SEQUENTIAL;
        score++;
      }
    } else if (index > 0 && !isalnum(string_get_char_at(line, index - 1))) {
      if (last_match_type != MATCH_TYPE_BOUNDARY) {
        last_match_type = MATCH_TYPE_BOUNDARY;
        score++;
      }
    } else {
      last_match_type = MATCH_TYPE_NORMAL;
      score += index - last_index + 1;
    }
    last_index = index + 1;
  }
  m->score = score;
  m->last = last_index;
  return true;
}

static bool regular_query(Score *score, String *query) {
  bool found_score = false;
  struct match m;
  for (size_t i = 0; i < score->line->length; i++) {
    if (string_get_ichar_at(score->line, i) == string_get_ichar_at(query, 0)) {
      if (find_end_of_match(&m, score->line, query, i)) {
        found_score = true;
        if (m.score < score->points) {
          score->points = m.score;
          score->last = m.last;
          score->first = i;
        }
      }
    }
  }
  return found_score;
}

Score *calculate_score(String *line, String *query) {
  Score *score = score_new();
  score->line = line;
  switch (query->length) {
  case 0:
    return score;
  case 1:
    if (single_char_query(score, query)) {
      return score;
    }
    break;
  default:
    if (regular_query(score, query)) {
      return score;
    }
  }
  free(score);
  return NULL;
}

void _score_log(const char *name, Score *score) {
  printf("[Score \"%s\" (%p)]", name, score);
  if (score != NULL) {
    printf(" points %" PRIu16 "; first %" PRIu16 "; last %" PRIu16
           "; line \"%s\"",
           score->points, score->first, score->last, score->line->buf);
  }
  printf("\n");
}

#ifdef TESTS
#include "test.h"

void test_score_new() {
  Score *score = score_new();
  test_assert(score->points == UINT16_MAX);
  test_assert(score->first == 0);
  test_assert(score->last == 0);
  test_assert(score->line == NULL);
}

void test_calculate_score() {
  String *s = string_new_from("foo");
  // verify NULL is returned if line doesn't contain query chars
  String *query = string_new_from("qxz");
  test_assert(calculate_score(s, query) == NULL);
  string_set(s, "oof");
  string_set(query, "f");
  Score *score1 = calculate_score(s, query);
  test_assert(score1->points == 1);
  test_assert(score1->first == 2);
  test_assert(score1->last == 3);
  test_assert(string_equals(score1->line, "oof"));
  free(score1);
  // this should be case insensitive
  string_set(query, "F");
  Score *score2 = calculate_score(s, query);
  test_assert(score2 != NULL);
  test_assert(score2->points == 1);
  test_assert(score2->first == 2);
  test_assert(score2->last == 3);
  free(score2);
  string_set(s, "foofbbar");
  string_set(query, "fob");
  Score *score3 = calculate_score(s, query);
  test_assert(score3->points == 5);
  test_assert(score3->first == 0);
  test_assert(score3->last == 5);
  free(score3);
  string_set(s, "foo / ba r");
  string_set(query, "or");
  Score *score4 = calculate_score(s, query);
  test_assert(score4->points == 2);
  test_assert(score4->first == 1);
  test_assert(score4->last == 10);
  free(score4);
  string_set(s, "f||||||||b||||||||||||||a||||f||||||||r");
  string_set(query, "bar");
  Score *score5 = calculate_score(s, query);
  test_assert(score5->points == 2);
  test_assert(score5->first == 9);
  test_assert(score5->last == 39);
  free(score5);
  // test that we get the best score
  string_set(s, "foofoobar");
  string_set(query, "foob");
  Score *score6 = calculate_score(s, query);
  test_assert(score6->points = 4);
  test_assert(score6->first = 3);
  test_assert(score6->last = 6);
  free(score6);
  // apparently there are no tests that would catch case sensitivity? dumb
  string_set(s, "FOOBAR");
  string_set(query, "foo");
  Score *score7 = calculate_score(s, query);
  test_assert(score7->first == 0);
  test_assert(score7->last == 3);
  test_assert(score7->points == 3);
  free(score7);
  string_free(s);
  string_free(query);
}

#endif
