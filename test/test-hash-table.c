#include "hash-table.h"
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
  hashtable_free(h);
}
