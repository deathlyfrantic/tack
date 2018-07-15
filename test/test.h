#ifndef _TEST_H
#define _TEST_H

#include <stdbool.h>
#include <string.h>

typedef struct {
  const char *assertion;
  const char *file;
  const char *func;
  int line;
} TestFailure;

#define assert(a)                                                              \
  _test_assertion(#a, a,                                                       \
                  strrchr(__FILE__, '/') != NULL ? strrchr(__FILE__, '/') + 1  \
                                                 : __FILE__,                   \
                  __LINE__, __func__)

void _test_assertion(const char *, bool, const char *, int, const char *);
void run_all_tests();

#endif /* end of include guard: _TEST_H */