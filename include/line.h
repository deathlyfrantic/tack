#ifndef TACK_LINE_H
#define TACK_LINE_H

typedef struct {
  char *original;
  char *lowered;
} Line;

Line *line_new();
void line_free(Line *);

#endif /* end of include guard: TACK_LINE_H */
