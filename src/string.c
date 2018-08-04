#include "tack-string.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define STRING_DEFAULT_CAPACITY 100

static void string_grow(String *s) {
  s->capacity += s->size;
  s->buf = realloc(s->buf, sizeof(char) * s->capacity);
  s->low = realloc(s->low, sizeof(char) * s->capacity);
}

String *string_new() {
  String *string = malloc(sizeof(String));
  string->length = 0;
  string->capacity = STRING_DEFAULT_CAPACITY;
  string->size = string->capacity;
  string->buf = calloc(string->capacity, sizeof(char));
  string->low = calloc(string->capacity, sizeof(char));
  return string;
}

String *string_new_from(char *source) {
  char c;
  String *string = string_new();
  while ((c = *source++)) {
    string_push_char(string, c);
  }
  return string;
}

String *string_dup(String *old) {
  return string_new_from(old->buf);
}

void string_push_char(String *s, char c) {
  if (s->length == s->capacity) {
    string_grow(s);
  }
  s->buf[s->length] = c;
  s->low[s->length] = tolower(c);
  s->length++;
}

char string_pop_char(String *s) {
  if (s->length == 0) return '\0';
  s->length--;
  char c = s->buf[s->length];
  s->buf[s->length] = '\0';
  s->low[s->length] = '\0';
  return c;
}

void string_concat(String *string, const char *s) {
  char c;
  while ((c = *s++)) {
    string_push_char(string, c);
  }
}

void string_delete_word(String *s) {
  if (s->length == 0) return;
  bool seen_char = false;
  char c;
  while ((c = string_pop_char(s))) {
    seen_char = seen_char || !isspace(c);
    if (seen_char && isspace(c)) {
      string_push_char(s, c);
      break;
    }
  }
}

void string_set(String *string, const char *s) {
  string_reset(string);
  string_concat(string, s);
}

void string_reset(String *s) {
  memset(s->buf, 0, s->length);
  memset(s->low, 0, s->length);
  s->length = 0;
}

bool string_equals(String *string, const char *s) {
  return strcmp(string->buf, s) == 0;
}

bool string_iequals(String *string, const char *s) {
  return strcasecmp(string->buf, s) == 0;
}

bool string_equals_string(String *s1, String *s2) {
  return strcmp(s1->buf, s2->buf) == 0;
}

bool string_iequals_string(String *s1, String *s2) {
  return strcmp(s1->low, s2->low) == 0;
}

void string_free(String *s) {
  if (s == NULL) return;
  free(s->buf);
  free(s->low);
  free(s);
}

int32_t string_find_char(String *s, char c) {
  for (size_t i = 0; i < s->length; i++) {
    if (s->buf[i] == c) {
      return i;
    }
  }
  return -1;
}

int32_t string_find_ichar(String *s, char c) {
  c = tolower(c);
  for (size_t i = 0; i < s->length; i++) {
    if (s->low[i] == c) {
      return i;
    }
  }
  return -1;
}

size_t string_count_chars(String *s, char c) {
  size_t rv = 0;
  for (size_t i = 0; i < s->length; i++) {
    if (s->buf[i] == c) {
      rv++;
    }
  }
  return rv;
}

size_t string_count_ichars(String *s, char c) {
  c = tolower(c);
  size_t rv = 0;
  for (size_t i = 0; i < s->length; i++) {
    if (s->low[i] == c) {
      rv++;
    }
  }
  return rv;
}

#ifdef TESTS
#include "test.h"

void test_string_new() {
  String *s = string_new();
  test_assert(s->capacity == STRING_DEFAULT_CAPACITY);
  test_assert(s->size == s->capacity);
  test_assert(s->length == 0);
  string_free(s);
}

void test_string_new_from() {
  String *s = string_new_from("FooBar");
  test_assert(strcmp(s->buf, "FooBar") == 0);
  test_assert(strcmp(s->low, "foobar") == 0);
  test_assert(s->length == 6);
  string_free(s);
}

void test_string_dup() {
  String *s1 = string_new_from("FooBar");
  String *s2 = string_dup(s1);
  test_assert(string_equals_string(s1, s2));
  test_assert(string_iequals_string(s1, s2));
  test_assert(&s1 != &s2);
  string_free(s1);
  string_free(s2);
}

void test_string_grow() {
  String *s = string_new();
  for (size_t i = 0; i < 150; i++) {
    string_push_char(s, 'x');
  }
  test_assert(s->capacity == STRING_DEFAULT_CAPACITY * 2);
  test_assert(s->length == 150);
  string_free(s);
}

void test_string_push_and_pop_char() {
  String *s = string_new();
  string_push_char(s, 'f');
  string_push_char(s, 'o');
  string_push_char(s, 'o');
  test_assert(s->length == 3);
  test_assert(strcmp(s->buf, "foo") == 0);
  test_assert(string_pop_char(s) == 'o');
  test_assert(s->length == 2);
  test_assert(string_pop_char(s) == 'o');
  test_assert(s->length == 1);
  test_assert(string_pop_char(s) == 'f');
  test_assert(s->length == 0);
  test_assert(string_pop_char(s) == '\0');
  test_assert(s->length == 0);
  string_free(s);
}

void test_string_set() {
  String *s = string_new();
  test_assert(strcmp(s->buf, "") == 0);
  test_assert(strcmp(s->low, "") == 0);
  test_assert(s->length == 0);
  string_set(s, "Foo");
  test_assert(strcmp(s->buf, "Foo") == 0);
  test_assert(strcmp(s->low, "foo") == 0);
  test_assert(s->length == 3);
  string_free(s);
}

void test_string_reset() {
  String *s = string_new_from("FooBar");
  string_reset(s);
  test_assert(strcmp(s->buf, "") == 0);
  test_assert(strcmp(s->low, "") == 0);
  test_assert(s->length == 0);
  string_free(s);
}

void test_string_equals() {
  String *s1 = string_new_from("FooBar");
  String *s2 = string_new_from("FooBar");
  test_assert(string_equals_string(s1, s2));
  test_assert(string_iequals_string(s1, s2));
  string_free(s2);
  String *s3 = string_new_from("fOoBaR");
  string_free(s3);
  test_assert(!string_equals_string(s1, s3));
  test_assert(string_iequals_string(s1, s3));
  test_assert(string_equals(s1, "FooBar"));
  test_assert(string_iequals(s1, "foobar"));
  test_assert(!string_iequals(s1, "jlaksjd"));
  string_free(s1);
}

void test_string_concat() {
  String *s = string_new_from("foobar");
  test_assert(s->length == 6);
  string_concat(s, "baz");
  test_assert(string_equals(s, "foobarbaz"));
  test_assert(s->length == 9);
  string_free(s);
}

void test_string_delete_word() {
  String *s = string_new_from(" Foo Bar Baz");
  string_delete_word(s);
  test_assert(string_equals(s, " Foo Bar "));
  string_delete_word(s);
  test_assert(string_iequals(s, " FOO "));
  string_delete_word(s);
  test_assert(string_equals(s, " "));
  string_delete_word(s);
  test_assert(string_equals(s, ""));
  string_free(s);
}

void test_string_find_char() {
  String *s = string_new_from("fooBarFoo");
  test_assert(string_find_char(s, 'f') == 0);
  test_assert(string_find_ichar(s, 'f') == 0);
  test_assert(string_find_char(s, 'F') == 6);
  test_assert(string_find_ichar(s, 'F') == 0);
  test_assert(string_find_char(s, 'x') == -1);
  test_assert(string_find_ichar(s, 'x') == -1);
  string_free(s);
}

void test_string_count_chars() {
  String *s = string_new_from("fooFOOfoo");
  test_assert(string_count_chars(s, 'f') == 2);
  test_assert(string_count_chars(s, 'o') == 4);
  test_assert(string_count_chars(s, 'x') == 0);
  test_assert(string_count_ichars(s, 'f') == 3);
  test_assert(string_count_ichars(s, 'o') == 6);
  test_assert(string_count_ichars(s, 'x') == 0);
  string_free(s);
}

#endif
