#ifndef TACK_TACK_H
#define TACK_TACK_H

#include <stdbool.h>

#define VERSION "0.0.1"

typedef struct {
  bool full_height;
  unsigned short height;
  char *initial_search;
} Config;

#endif /* end of include guard: TACK_TACK_H */
