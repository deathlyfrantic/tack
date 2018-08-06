#ifndef TACK_STRING_H
#define TACK_STRING_H

#include "charmap.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  char *buf;
  char *low;
  size_t length;
  size_t capacity;
  size_t size;
  CharMap *charmap;
  CharMap *lowcharmap;
} String;

String *string_new();
String *string_new_from(char *);
String *string_dup(String *);
const char *string_raw(String *);
const char *string_raw_low(String *);
char string_get_char_at(String *, size_t);
char string_get_ichar_at(String *, size_t);
void string_push_char(String *, char);
char string_pop_char(String *);
void string_concat(String *, const char *);
void string_delete_word(String *);
void string_set(String *, const char *);
void string_reset(String *);
bool string_equals(String *, const char *);
bool string_iequals(String *, const char *);
bool string_equals_string(String *, String *);
bool string_iequals_string(String *, String *);
void string_free(String *);
int32_t string_find_char(String *, char);
int32_t string_find_ichar(String *, char);
int32_t string_find_char_from(String *, char, size_t);
int32_t string_find_ichar_from(String *, char, size_t);
size_t string_count_chars(String *, char);
size_t string_count_ichars(String *, char);

#endif /* end of include guard: TACK_STRING_H */
