#ifndef TACK_SCORE_H
#define TACK_SCORE_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint16_t points;
  uint16_t first;
  uint16_t last;
  const char *line;
} Score;

Score *score_new();
Score *calculate_score(const char *, const char *);
void _score_log(const char *, Score *);

#define score_log(a) _score_log(#a, a);

#endif /* end of include guard: TACK_SCORE_H */
