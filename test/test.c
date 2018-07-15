#include "list.h"
#include "test.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

List *failures;
size_t num_tests;

void _test_assertion(const char *assertion, bool passed, const char *file,
                     int line, const char *func) {
  num_tests++;
  if (passed) {
    printf(".");
  } else {
    printf("F");
    TestFailure *failure = malloc(sizeof(TestFailure));
    failure->assertion = assertion;
    failure->line = line;
    failure->file = file;
    failure->func = func;
    list_push(failures, failure);
  }
}

int main(int argc, char *argv[]) {
  num_tests = 0;
  failures = list_new();
  printf("Starting tests.\n");
  run_all_tests();
  printf("\n\n");
  printf("Total tests: %zd, passed: %zd, failed: %zd\n", num_tests,
         num_tests - failures->length, failures->length);
  if (failures->length > 0) {
    printf("\n"
           "FAILURES:\n"
           "--------\n");
    for (size_t i = 0; i < failures->length; i++) {
      TestFailure *failure = failures->items[i];
      printf("[%s:%d:%s()] - \"%s\"", failure->file, failure->line,
             failure->func, failure->assertion);
    }
    printf("\n");
  }
}
