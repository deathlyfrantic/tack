#ifndef TACK_CONFIG_H
#define TACK_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool full_height;
  uint16_t height;
  char *initial_search;
} Config;

Config *config_new();
void config_free(Config *);

#endif /* end of include guard: TACK_CONFIG_H */
