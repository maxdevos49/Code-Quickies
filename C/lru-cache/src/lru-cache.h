#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <stddef.h>
#include <stdbool.h>

typedef struct LRUCache LRUCACHE;

LRUCACHE* lru_cache_create(size_t capacity);
void lru_cache_destroy(LRUCACHE* cache);

void lru_cache_put(LRUCACHE*cache, int x, int z, void* value);
void* lru_cache_get(LRUCACHE*cache, int x, int z);


#endif
