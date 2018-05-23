#include "score.h"
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Score *score_new() {
  Score *score = malloc(sizeof(Score));
  score->points = UINT_MAX;
  score->first = 0;
  score->last = 0;
  score->line = NULL;
  return score;
}

static char *strtolower(const char *s) {
  char *rv = calloc(strlen(s), sizeof(char));
  for (size_t i = 0; i < strlen(s); i++) {
    rv[i] = tolower(s[i]);
  }
  return rv;
}

static int find_char_idx(const char *s, char c) {
  int rv = -1;
  for (size_t i = 0; i < strlen(s); i++) {
    if (s[i] == c) {
      rv = i;
      break;
    }
  }
  return rv;
}

static bool single_char_query(Score *score, const char *query) {
  int idx;
  char *line = strtolower(score->line);
  if ((idx = find_char_idx(line, query[0])) == -1) {
    return false;
  }
  score->first = idx;
  score->last = idx;
  score->points = 1;
  free(line);
  return true;
}

struct match {
  unsigned int score;
  unsigned int last;
};

enum match_type {
  MATCH_TYPE_SEQUENTIAL,
  MATCH_TYPE_BOUNDARY,
  MATCH_TYPE_NORMAL,
};

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
    last_index = index + 1;
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
    break;

  case 1:
    if (!single_char_query(score, query)) {
      goto no_match;
    }

  default:
    if (!regular_query(score, query)) {
      goto no_match;
    }
  }

  return score;

no_match:
  free(score);
  return NULL;
}

void _score_log(const char *name, Score *score) {
  printf("[Score \"%s\" (%p)]", name, score);
  if (score != NULL) {
    printf(" points %d; first %d; last %d; line \"%s\"\n", score->points,
           score->first, score->last, score->line);
  } else {
    printf("\n");
  }
}

// int main(int argc, char *argv[]) {
//   Score *a = calculate_score("foo", "qxz");
//   printf("query: \"qxz\"\nshould be: none\n");
//   score_log(a);
//   printf("\n\n");

//   Score *b = calculate_score("oof", "f");
//   printf("query: \"f\"\nshould be: P1 F2 L2\n");
//   score_log(b);
//   printf("\n\n");

//   Score *c = calculate_score("foofbbar", "fob");
//   printf("query: \"fob\"\nshould be: P5 F0 L4\n");
//   score_log(c);
//   printf("\n\n");

//   Score *d = calculate_score("foo / ba r", "or");
//   printf("query: \"or\"\nshould be: P2 F1 L9\n");
//   score_log(d);
//   printf("\n\n");

//   Score *e = calculate_score("f||||||||b||||||||||||||a||||f||||||||r",
//   "bar"); printf("query: \"bar\"\nshould be: P2 F9 L38\n"); score_log(e);
//   printf("\n\n");

//   return EXIT_SUCCESS;
// }
