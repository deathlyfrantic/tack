#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

char *strtolower(const char *s) {
  char *rv = strdup(s);
  for (size_t i = 0; i < strlen(rv); i++) {
    rv[i] = tolower(rv[i]);
  }
  return rv;
}

unsigned int find_char_idx(const char *s, char c) {
  for (size_t i = 0; i < strlen(s); i++) {
    if (s[i] == c) {
      return i;
    }
  }
  return -1;
}

unsigned short get_num_strlen(size_t num) {
  char buf[USHRT_MAX] = {0};
  sprintf(buf, "%zd", num);
  return strlen(buf);
}
