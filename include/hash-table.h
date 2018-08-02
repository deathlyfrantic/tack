#ifndef TACK_HASH_TABLE_H
#define TACK_HASH_TABLE_H

#include <stdint.h>

typedef struct hashtableentry {
  struct hashtableentry *next;
  uint32_t hash;
  void *item;
} HashTableEntry;

typedef struct {
  uint32_t (*hash)(const void *);
  HashTableEntry **buckets;
  uint8_t size;
} HashTable;

HashTable *hashtable_new();
void hashtable_set(HashTable *, const void *, void *);
void *hashtable_get(HashTable *, const void *);
void hashtable_free(HashTable *);
void hashtable_free_items(HashTable *, void (*)(void *));

#endif /* end of include guard: TACK_HASH_TABLE_H */
