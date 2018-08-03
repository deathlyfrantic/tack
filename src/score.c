#include "line.h"
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

static bool single_char_query(Score *score, const char *query) {
  int16_t idx;
  bool rv = false;
  if ((idx = find_char_idx(score->line->lowered, tolower(query[0]))) != -1) {
    score->first = idx;
    score->last = idx + 1;
    score->points = 1;
    rv = true;
  }
  return rv;
}

static bool find_end_of_match(struct match *m, const char *line,
                              const char *query, size_t start) {
  uint16_t last_index = start, score = 1;
  int16_t index;
  enum match_type last_match_type = MATCH_TYPE_NORMAL;
  for (size_t i = 0; i < strlen(query); i++) {
    if ((index = find_char_idx(line + last_index, query[i])) == -1) {
      return false;
    }
    index += last_index;
    if (index == last_index + 1) {
      if (last_match_type != MATCH_TYPE_SEQUENTIAL) {
        last_match_type = MATCH_TYPE_SEQUENTIAL;
        score++;
      }
    } else if (index > 0 && !isalnum(line[index - 1])) {
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

static bool regular_query(Score *score, const char *_query) {
  bool found_score = false;
  struct match m;
  char *query = strtolower(_query);
  for (size_t i = 0; i < strlen(score->line->lowered); i++) {
    if (score->line->lowered[i] == query[0]) {
      if (find_end_of_match(&m, score->line->lowered + 1, query + 1, i)) {
        found_score = true;
        if (m.score < score->points) {
          score->points = m.score;
          score->last = m.last + 1;
          score->first = i;
        }
      }
    }
  }
  free(query);
  return found_score;
}

Score *calculate_score(Line *line, const char *query) {
  Score *score = score_new();
  score->line = line;
  switch (strlen(query)) {
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
           score->points, score->first, score->last, score->line->original);
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
  Line *line = line_new();
  line->original = "foo";
  line->lowered = "foo";
  // verify NULL is returned if line doesn't contain query chars
  test_assert(calculate_score(line, "qxz") == NULL);
  line->original = "oof";
  line->lowered = "oof";
  Score *score1 = calculate_score(line, "f");
  test_assert(score1->points == 1);
  test_assert(score1->first == 2);
  test_assert(score1->last == 3);
  test_assert(strcmp(score1->line->original, "oof") ==
              0); // just to verify line gets set
  free(score1);
  // this should be case insensitive
  Score *score2 = calculate_score(line, "F");
  test_assert(score2 != NULL);
  test_assert(score2->points == 1);
  test_assert(score2->first == 2);
  test_assert(score2->last == 3);
  free(score2);
  line->original = "foofbbar";
  line->lowered = "foofbbar";
  Score *score3 = calculate_score(line, "fob");
  test_assert(score3->points == 5);
  test_assert(score3->first == 0);
  test_assert(score3->last == 5);
  free(score3);
  line->original = "foo / ba r";
  line->lowered = "foo / ba r";
  Score *score4 = calculate_score(line, "or");
  test_assert(score4->points == 2);
  test_assert(score4->first == 1);
  test_assert(score4->last == 10);
  free(score4);
  line->original = "f||||||||b||||||||||||||a||||f||||||||r";
  line->lowered = "f||||||||b||||||||||||||a||||f||||||||r";
  Score *score5 = calculate_score(line, "bar");
  test_assert(score5->points == 2);
  test_assert(score5->first == 9);
  test_assert(score5->last == 39);
  free(score5);
  // test that we get the best score
  line->original = "foofoobar";
  line->lowered = "foofoobar";
  Score *score6 = calculate_score(line, "foob");
  test_assert(score6->points = 4);
  test_assert(score6->first = 3);
  test_assert(score6->last = 6);
  free(score6);
  free(line);
}

#endif
