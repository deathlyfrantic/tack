#include "hash-table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHTABLE_DEFAULT_SIZE 20

static uint32_t hash_string(const void *_s) {
  const char *s = _s;
  uint32_t hash = 0;
  char c;
  while ((c = *s++)) {
    // I just made this up, who knows if it's any good
    hash += (hash << 3) + c;
  }
  return hash;
}

HashTable *hashtable_new() {
  HashTable *rv = malloc(sizeof(HashTable));
  rv->size = HASHTABLE_DEFAULT_SIZE;
  rv->buckets = calloc(HASHTABLE_DEFAULT_SIZE, sizeof(HashTableEntry));
  rv->hash = hash_string;
  return rv;
}

void hashtable_set(HashTable *h, const void *key, void *value) {
  const uint32_t hash = h->hash(key);
  size_t bucket = hash % h->size;
  HashTableEntry *entry = h->buckets[bucket], *prev = NULL;
  while (entry != NULL && entry->hash != hash) {
    prev = entry;
    entry = entry->next;
  }
  if (entry == NULL) {
    entry = malloc(sizeof(HashTableEntry));
    entry->hash = hash;
    entry->next = NULL;
    if (prev != NULL) {
      prev->next = entry;
    } else {
      h->buckets[bucket] = entry;
    }
  }
  entry->item = value;
}

void *hashtable_get(HashTable *h, const void *key) {
  void *rv = NULL;
  const uint32_t hash = h->hash(key);
  HashTableEntry *entry = h->buckets[hash % h->size];
  while (entry != NULL && entry->hash != hash) {
    entry = entry->next;
  }
  if (entry != NULL) {
    rv = entry->item;
  }
  return rv;
}

void hashtable_free(HashTable *h) {
  if (h == NULL) return;
  HashTableEntry *entry = NULL, *next = NULL;
  for (size_t i = 0; i < h->size; i++) {
    entry = h->buckets[i];
    while (entry != NULL) {
      next = entry->next;
      free(entry);
      entry = next;
    }
  }
  free(h->buckets);
  free(h);
}

void hashtable_free_items(HashTable *h, void (*free_function)(void *)) {
  if (h == NULL) return;
  HashTableEntry *entry = NULL;
  for (size_t i = 0; i < h->size; i++) {
    entry = h->buckets[i];
    while (entry != NULL) {
      free_function(entry->item);
      entry = entry->next;
    }
  }
}

#ifdef TESTS
#include "test.h"

void test_hashtable() {
  // there's no real way to test individual hash table functions so just test
  // everything in one go
  HashTable *h = hashtable_new();
  hashtable_set(h, "foo", strdup("bar"));
  test_assert(strcmp(hashtable_get(h, "foo"), "bar") == 0);
  hashtable_set(h, "foo", strdup("baz"));
  test_assert(strcmp(hashtable_get(h, "foo"), "baz") == 0);
  test_assert(hashtable_get(h, "foobar") == NULL);
  hashtable_free_items(h, free);
  hashtable_free(h);
}

#endif
