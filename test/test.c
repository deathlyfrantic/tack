#include "list.h"
#include "test.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

List *results;

void _test_assertion(const char *assertion, bool passed, const char *file,
                     int line, const char *func) {
  TestResult *result = malloc(sizeof(TestResult));
  result->line = line;
  result->file = file;
  result->func = func;
  result->passed = passed;
  result->assertion = assertion;
  printf(passed ? "." : "F");
  list_push(results, result);
}

static bool filter_successes(const void *_result) {
  TestResult *result = (TestResult *)_result;
  return result->passed;
}

static bool filter_failures(const void *result) {
  return !filter_successes(result);
}

int main(int argc, char *argv[]) {
  results = list_new();
  puts("Starting tests.");
  run_all_tests();
  List *successes = list_filter(results, filter_successes);
  List *failures = list_filter(results, filter_failures);
  puts("\n");
  printf("Total tests: %zd, passed: %zd, failed: %zd\n", results->length,
         successes->length, failures->length);
  int exit_code = EXIT_SUCCESS;
  TestResult *r;
  if (failures->length > 0) {
    puts("\nFAILURES:\n---------");
    for (size_t i = 0; i < failures->length; i++) {
      r = failures->items[i];
      printf("[%s:%d:%s()] - \"%s\"\n", r->file, r->line, r->func,
             r->assertion);
    }
    puts("");
    exit_code = EXIT_FAILURE;
  }
  for (size_t i = 0; i < results->length; i++) {
    r = results->items[i];
    free(r);
  }
  list_free(successes);
  list_free(failures);
  list_free(results);
  return exit_code;
}
