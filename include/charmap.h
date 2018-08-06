#ifndef TACK_CHARMAP_H
#define TACK_CHARMAP_H

#include "hash-table.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  char c;
  size_t length;
  size_t capacity;
  size_t *positions;
} CharMapEntry;

typedef HashTable CharMap;

CharMap *charmap_new();
size_t charmap_get_position_after(CharMap *, char, size_t);
void charmap_add_position(CharMap *, char, size_t);
void charmap_delete_position(CharMap *, char, size_t);
void charmap_reset(CharMap *);
void charmap_free(CharMap *);

#endif /* end of include guard: TACK_CHARMAP_H */
