#include "line.h"
#include <stdlib.h>

Line *line_new() {
  Line *line = malloc(sizeof(Line));
  line->original = NULL;
  line->lowered = NULL;
  return line;
}

void line_free(Line *line) {
  if (line == NULL) return;
  free(line->original);
  free(line->lowered);
  free(line);
}
