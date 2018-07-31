#include "config.h"
#include <stdlib.h>
#include <string.h>

Config *config_init() {
  Config *config = malloc(sizeof(Config));
  config->full_height = false;
  config->height = 21;
  config->initial_search = strdup("");
  return config;
}

void config_free(Config *config) {
  free(config->initial_search);
  free(config);
}
