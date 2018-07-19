#include "score.h"
#include "util.h"
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct match {
  unsigned int score;
  unsigned int last;
};

enum match_type {
  MATCH_TYPE_SEQUENTIAL,
  MATCH_TYPE_BOUNDARY,
  MATCH_TYPE_NORMAL,
};

Score *score_new() {
  Score *score = malloc(sizeof(Score));
  score->points = UINT_MAX;
  score->first = 0;
  score->last = 0;
  score->line = NULL;
  return score;
}

static bool single_char_query(Score *score, const char *query) {
  unsigned int idx;
  char *line = strtolower(score->line);
  bool rv = false;
  if ((idx = find_char_idx(line, tolower(query[0]))) != -1) {
    score->first = idx;
    score->last = idx;
    score->points = 1;
    rv = true;
  }
  free(line);
  return rv;
}

static bool find_end_of_match(struct match *m, const char *line,
                              const char *query, unsigned int start) {
  unsigned int last_index = start, index;
  unsigned int score = 1;
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
      score += index - last_index;
    }
    last_index = index;
  }
  m->score = score;
  m->last = last_index;
  return true;
}

static bool regular_query(Score *score, const char *_query) {
  bool found_score = false;
  struct match m;
  char *line = strtolower(score->line);
  char *query = strtolower(_query);
  for (size_t i = 0; i < strlen(line); i++) {
    if (line[i] == query[0]) {
      if (find_end_of_match(&m, line, query + 1, i)) {
        found_score = true;
        if (m.score < score->points) {
          score->points = m.score;
          score->last = m.last;
          score->first = i;
        }
      }
    }
  }
  free(line);
  free(query);
  return found_score;
}

Score *calculate_score(const char *line, const char *query) {
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
    printf(" points %d; first %d; last %d; line \"%s\"", score->points,
           score->first, score->last, score->line);
  }
  printf("\n");
}
