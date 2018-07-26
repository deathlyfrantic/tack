#ifndef TACK_UTIL_H
#define TACK_UTIL_H

#include <stdlib.h>

#define MIN(a, b) ((a < b) ? (a) : (b))

char *strtolower(const char *);
unsigned int find_char_idx(const char *, char);
unsigned short get_num_strlen(size_t);
size_t count_chars_in_string(const char *, char);

#endif /* end of include guard: TACK_UTIL_H */
