#include "hash-table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 20

static unsigned int hash_string(const void *_s) {
  const char *s = _s;
  unsigned int hash = 0;
  char c;
  while ((c = *s++)) {
    // I just made this up, who knows if it's any good
    hash += (hash + c) << 3;
  }
  return hash;
}

HashTable *hashtable_new() {
  HashTable *rv = malloc(sizeof(HashTable));
  rv->size = DEFAULT_SIZE;
  rv->buckets = calloc(DEFAULT_SIZE, sizeof(HashTableEntry));
  rv->hash = hash_string;
  return rv;
}

void hashtable_set(HashTable *h, const void *key, void *value) {
  const unsigned int hash = h->hash(key);
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
  const unsigned int hash = h->hash(key);
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
  HashTableEntry *entry = NULL;
  for (size_t i = 0; i < h->size; i++) {
    entry = h->buckets[i];
    while (entry != NULL) {
      free_function(entry->item);
      entry = entry->next;
    }
  }
}
