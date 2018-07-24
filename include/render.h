#ifndef TACK_RENDER_H
#define TACK_RENDER_H

#include "list.h"
#include <stdbool.h>
#include <stdlib.h>

// clang-format off
#define COLOR_DEFAULT      "\e[39m"
#define COLOR_BLACK        "\e[30m"
#define COLOR_RED          "\e[31m"
#define COLOR_GREEN        "\e[32m"
#define COLOR_YELLOW       "\e[33m"
#define COLOR_BLUE         "\e[34m"
#define COLOR_MAGENTA      "\e[35m"
#define COLOR_CYAN         "\e[36m"
#define COLOR_BLACK_BOLD   "\e[30;1m"
#define COLOR_RED_BOLD     "\e[31;1m"
#define COLOR_GREEN_BOLD   "\e[32;1m"
#define COLOR_YELLOW_BOLD  "\e[33;1m"
#define COLOR_BLUE_BOLD    "\e[34;1m"
#define COLOR_MAGENTA_BOLD "\e[35;1m"
#define COLOR_CYAN_BOLD    "\e[36;1m"
#define COLOR_RESET        "\e[0m"
#define COLOR_REVERSE      "\e[7m"
#define HIDE_CURSOR        "\e[?25l"
#define SHOW_CURSOR        "\e[?25h"
#define CLEAR_LINE         "\e[0K"
// clang-format on

typedef struct {
  List *scores;
  const char *query;
  unsigned short match_length;
  unsigned short height;
  unsigned short selected;
} Renderer;

char *highlight_line(const char *, size_t, size_t, bool, bool);
char *expand_tabs(const char *);
Renderer *renderer_new();
char *renderer_render(Renderer *);

#endif /* end of include guard: TACK_RENDER_H */
