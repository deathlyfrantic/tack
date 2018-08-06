#include "tack-string.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define STRING_DEFAULT_CAPACITY 100

static void string_grow(String *s, size_t needed) {
  while (s->capacity < needed) {
    s->capacity += s->size;
  }
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
  String *string = string_new();
  string_concat(string, source);
  return string;
}

const char *string_raw(String *s) {
  return s->buf;
}

const char *string_raw_low(String *s) {
  return s->low;
}

char string_get_char_at(String *s, size_t i) {
  if (i > s->length) return '\0';
  return s->buf[i];
}

char string_get_ichar_at(String *s, size_t i) {
  if (i > s->length) return '\0';
  return s->low[i];
}

void string_push_char(String *s, char c) {
  if (s->length == s->capacity) {
    string_grow(s, s->capacity * 2);
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
  size_t strlen_s = strlen(s);
  char lowered[strlen_s + 1];
  memset(&lowered, 0, strlen_s + 1);
  for (size_t i = 0; i < strlen_s; i++) {
    lowered[i] = tolower(s[i]);
  }
  if (string->length + strlen_s > string->capacity) {
    string_grow(string, string->length + strlen_s);
  }
  strcat(string->buf, s);
  strcat(string->low, lowered);
  string->length += strlen_s;
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

void string_free(String *s) {
  if (s == NULL) return;
  free(s->buf);
  free(s->low);
  free(s);
}

static int32_t find_char(const char *s, size_t length, char c) {
  for (size_t i = 0; i < length; i++) {
    if (s[i] == c) {
      return i;
    }
  }
  return -1;
}

int32_t string_find_char(String *s, char c) {
  return find_char(s->buf, s->length, c);
}

int32_t string_find_ichar(String *s, char c) {
  return find_char(s->low, s->length, tolower(c));
}

int32_t string_find_char_from(String *s, char c, size_t start) {
  if (start > s->length) return -1;
  return find_char(s->buf + start, s->length - start, c);
}

int32_t string_find_ichar_from(String *s, char c, size_t start) {
  if (start > s->length) return -1;
  return find_char(s->low + start, s->length - start, tolower(c));
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
  String *s = string_new_from("FooBar");
  test_assert(string_equals(s, "FooBar"));
  test_assert(!string_equals(s, "BazQuux"));
  string_free(s);
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
  test_assert(string_equals(s, " Foo "));
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
  test_assert(string_find_char_from(s, 'o', 3) == 4);
  test_assert(string_find_char_from(s, 'o', 2) == 0);
  test_assert(string_find_ichar_from(s, 'f', 1) == 5);
  test_assert(string_find_char_from(s, 'x', 3) == -1);
  string_free(s);
}

void test_string_count_chars() {
  String *s = string_new_from("fooFOOfoo");
  test_assert(string_count_chars(s, 'f') == 2);
  test_assert(string_count_chars(s, 'o') == 4);
  test_assert(string_count_chars(s, 'x') == 0);
  string_free(s);
}

#endif
