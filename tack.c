#include "list.h"
#include "score.h"
#include "tack.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

List *get_lines_from_stdin() {
  char line[BUFSIZ];
  unsigned int cursor = 0;
  char c;
  List *lines = list_new_of_size(100);
  bool keep_going = true;

  flockfile(stdin);

  do {
    switch ((c = getchar_unlocked())) {
    case EOF:
      keep_going = false;
      if (strcmp(line, "") == 0) {
        break;
      }

    case '\n':
      line[cursor] = '\0';
      cursor = 0;
      list_push(lines, strdup(line));
      memset(line, 0, sizeof(char) * BUFSIZ);
      break;

    default:
      line[cursor] = c;
      cursor++;
      break;
    }
  } while (keep_going);

  funlockfile(stdin);
  return lines;
}

static bool filter_out_null_scores(const void *score) { return score != NULL; }

static void *map_score(void *line) {
  return (void *)calculate_score((char *)line, "foobar");
}

int main(int argc, char *argv[]) {
  List *lines = get_lines_from_stdin();
  // for (size_t i = 0; i < lines->length; i++) {
  //   printf("line %zd: %s\n", i, lines->items[i]);
  // }
  printf("lines read: %zd\n", lines->length);

  // List *scores = list_new_of_size(100);

  // for (size_t i = 0; i < lines->length; i++) {
  //   Score *score = calculate_score(lines->items[i], "foobar");
  //   if (score != NULL) {
  //     list_push(scores, score);
  //   }
  // }

  List *scores =
      list_filter(list_map(lines, map_score), filter_out_null_scores);

  printf("lines matched \"foobar\": %zd\n", scores->length);
  // Score *score;
  // for (size_t i = 0; i < scores->length; i++) {
  //   score = scores->items[i];
  //   for (size_t j = 0; j < strlen(score->line); j++) {
  //     if (score->first == j || score->last == j - 1) {
  //       printf(" <|> ");
  //     }
  //     printf("%c", score->line[j]);
  //   }
  //   printf("\n");
  // }
  free(lines);
  return EXIT_SUCCESS;
}
