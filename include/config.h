#ifndef TACK_CONFIG_H
#define TACK_CONFIG_H

#include <stdbool.h>

typedef struct {
  bool full_height;
  unsigned short height;
  char *initial_search;
} Config;

Config *config_new();
void config_free(Config *);

#endif /* end of include guard: TACK_CONFIG_H */
