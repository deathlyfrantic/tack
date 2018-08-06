#include "charmap.h"
#include "hash-table.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CHARMAP_ENTRY_DEFAULT_SIZE 10
#define CHARMAP_DEFAULT_NUM_BUCKETS 255

static uint32_t hash_char(const void *c) {
  return *(char *)c;
}

static void charmap_entry_grow(CharMapEntry *entry) {
  entry->capacity += CHARMAP_ENTRY_DEFAULT_SIZE;
  entry->positions =
    realloc(entry->positions, entry->capacity * sizeof(size_t));
}

static CharMapEntry *charmap_entry_new() {
  CharMapEntry *entry = malloc(sizeof(CharMapEntry));
  entry->length = 0;
  entry->capacity = CHARMAP_ENTRY_DEFAULT_SIZE;
  entry->positions = calloc(CHARMAP_ENTRY_DEFAULT_SIZE, sizeof(size_t));
  return entry;
}

static void charmap_entry_add_position(CharMapEntry *entry, size_t position) {
  if (entry->length == entry->capacity) {
    charmap_entry_grow(entry);
  }
  entry->positions[entry->length++] = position;
}

static size_t charmap_entry_get_position_after(CharMapEntry *entry,
                                               size_t position) {
  for (size_t i = 0; i < entry->length; i++) {
    if (entry->positions[i] > position) {
      return entry->positions[i];
    }
  }
  // we can't return -1 for size_t so use 0 as a sentinel,
  // we'll never care about 0 in this program
  return 0;
}

static void charmap_entry_delete_position(CharMapEntry *entry,
                                          size_t position) {
  size_t i = entry->length;
  while (i--) {
    // move in reverse because we're _probably_ deleting the last position here
    if (entry->positions[i] == position) {
      memmove(&entry->positions[i], &entry->positions[i + 1],
              sizeof(size_t) * (entry->length - i));
      entry->positions[entry->length - 1] = 0;
      entry->length--;
    }
  }
}

static void charmap_entry_reset(CharMapEntry *entry) {
  for (size_t i = 0; i < entry->length; i++) {
    entry->positions[i] = 0;
  }
  entry->length = 0;
}

static void charmap_entry_free(void *_entry) {
  if (_entry == NULL) return;
  CharMapEntry *entry = (CharMapEntry *)_entry;
  free(entry->positions);
  free(entry);
}

CharMap *charmap_new() {
  CharMap *table = malloc(sizeof(CharMap));
  table->size = CHARMAP_DEFAULT_NUM_BUCKETS;
  table->buckets = calloc(CHARMAP_DEFAULT_NUM_BUCKETS, sizeof(CharMapEntry));
  table->hash = hash_char;
  return table;
}

size_t charmap_get_position_after(CharMap *map, char c, size_t pos) {
  CharMapEntry *entry = hashtable_get(map, &c);
  if (entry == NULL) return 0;
  return charmap_entry_get_position_after(entry, pos);
}

void charmap_add_position(CharMap *map, char c, size_t pos) {
  CharMapEntry *entry = hashtable_get(map, &c);
  if (entry == NULL) {
    entry = charmap_entry_new();
    hashtable_set(map, &c, entry);
  }
  charmap_entry_add_position(entry, pos);
}

void charmap_delete_position(CharMap *map, char c, size_t pos) {
  CharMapEntry *entry = hashtable_get(map, &c);
  if (entry == NULL) return;
  charmap_entry_delete_position(entry, pos);
}

void charmap_reset(CharMap *map) {
  HashTableEntry *entry = NULL;
  for (size_t i = 0; i < map->size; i++) {
    entry = map->buckets[i];
    while (entry != NULL) {
      charmap_entry_reset(entry->item);
      entry = entry->next;
    }
  }
}

void charmap_free(CharMap *map) {
  hashtable_free_items(map, charmap_entry_free);
  hashtable_free(map);
}

#ifdef TESTS
#include "test.h"

void test_charmap_entry_new() {
  CharMapEntry *entry = charmap_entry_new();
  test_assert(entry->capacity == CHARMAP_ENTRY_DEFAULT_SIZE);
  test_assert(entry->length == 0);
  for (size_t i = 0; i < entry->length; i++) {
    test_assert(entry->positions[i] == 0);
  }
  charmap_entry_free(entry);
}

void test_charmap_entry_grow() {
  CharMapEntry *entry = charmap_entry_new();
  for (size_t i = 0; i < 15; i++) {
    charmap_entry_add_position(entry, i);
  }
  test_assert(entry->length == 15);
  test_assert(entry->capacity == CHARMAP_ENTRY_DEFAULT_SIZE * 2);
  charmap_entry_free(entry);
}

void test_charmap_entry_get_position_after() {
  CharMapEntry *entry = charmap_entry_new();
  for (size_t i = 0; i < 15; i++) {
    charmap_entry_add_position(entry, i);
  }
  test_assert(charmap_entry_get_position_after(entry, 4) == 5);
  test_assert(charmap_entry_get_position_after(entry, 27) == 0);
  charmap_entry_free(entry);
}

void test_charmap_entry_delete_position() {
  CharMapEntry *entry = charmap_entry_new();
  for (size_t i = 0; i < 15; i++) {
    charmap_entry_add_position(entry, i);
  }
  charmap_entry_delete_position(entry, 7);
  test_assert(entry->positions[0] == 0);
  test_assert(entry->positions[1] == 1);
  test_assert(entry->positions[2] == 2);
  test_assert(entry->positions[3] == 3);
  test_assert(entry->positions[4] == 4);
  test_assert(entry->positions[5] == 5);
  test_assert(entry->positions[6] == 6);
  test_assert(entry->positions[7] == 8);
  test_assert(entry->positions[8] == 9);
  test_assert(entry->positions[9] == 10);
  test_assert(entry->positions[10] == 11);
  test_assert(entry->positions[11] == 12);
  test_assert(entry->positions[12] == 13);
  test_assert(entry->positions[13] == 14);
  test_assert(entry->positions[14] == 0);
  test_assert(entry->length == 14);
  charmap_entry_free(entry);
}

void test_charmap_get_position_after() {
  CharMap *map = charmap_new();
  charmap_add_position(map, 'f', 0);
  charmap_add_position(map, 'o', 1);
  charmap_add_position(map, 'o', 2);
  charmap_add_position(map, 'b', 3);
  charmap_add_position(map, 'a', 4);
  charmap_add_position(map, 'r', 5);
  test_assert(charmap_get_position_after(map, 'o', 0) == 1);
  test_assert(charmap_get_position_after(map, 'o', 1) == 2);
  test_assert(charmap_get_position_after(map, 'r', 0) == 5);
  test_assert(charmap_get_position_after(map, 'x', 0) == 0);
}

#endif
