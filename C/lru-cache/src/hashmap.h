#ifndef HASHMAP_H
#define HASHMAP_H 1

#include <stddef.h>
#include <stdbool.h>

typedef struct Hashmap HASHMAP;

HASHMAP *hashmap_create(size_t capacity);
void hashmap_destroy(HASHMAP *hashmap);

bool hashmap_insert(HASHMAP *hashmap, int x, int z, void *value);
void *hashmap_remove(HASHMAP *hashmap, int x, int z);
void *hashmap_get(HASHMAP *hashmap, int x, int z);
size_t hashmap_size(HASHMAP *hashmap);

#endif
