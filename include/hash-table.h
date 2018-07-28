#ifndef TACK_HASH_TABLE_H
#define TACK_HASH_TABLE_H

typedef struct hashtableentry {
  struct hashtableentry *next;
  unsigned int hash;
  void *item;
} HashTableEntry;

typedef struct {
  unsigned int (*hash)(const void *);
  HashTableEntry **buckets;
  unsigned int size;
} HashTable;

HashTable *hashtable_new();
void hashtable_set(HashTable *, const void *, void *);
void *hashtable_get(HashTable *, const void *);
void hashtable_free(HashTable *);

#endif /* end of include guard: TACK_HASH_TABLE_H */
