#ifndef TACK_RENDER_H
#define TACK_RENDER_H

#include "list.h"
#include <stdbool.h>
#include <stdlib.h>

// clang-format off
#define COLOR_DEFAULT      "\x1b[39m"
#define COLOR_BLACK        "\x1b[30m"
#define COLOR_RED          "\x1b[31m"
#define COLOR_GREEN        "\x1b[32m"
#define COLOR_YELLOW       "\x1b[33m"
#define COLOR_BLUE         "\x1b[34m"
#define COLOR_MAGENTA      "\x1b[35m"
#define COLOR_CYAN         "\x1b[36m"
#define COLOR_BLACK_BOLD   "\x1b[30;1m"
#define COLOR_RED_BOLD     "\x1b[31;1m"
#define COLOR_GREEN_BOLD   "\x1b[32;1m"
#define COLOR_YELLOW_BOLD  "\x1b[33;1m"
#define COLOR_BLUE_BOLD    "\x1b[34;1m"
#define COLOR_MAGENTA_BOLD "\x1b[35;1m"
#define COLOR_CYAN_BOLD    "\x1b[36;1m"
#define COLOR_RESET        "\x1b[0m"
#define COLOR_REVERSE      "\x1b[7m"
// clang-format on

typedef struct {
  List *scores;
  const char *query;
  unsigned short match_length;
  unsigned short height;
  unsigned short selected;
} Renderer;

char *highlight_line(const char *, size_t, size_t, bool);
char *expand_tabs(const char *);
Renderer *renderer_new();
char *renderer_render(Renderer *);

#endif /* end of include guard: TACK_RENDER_H */
