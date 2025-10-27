#include "lru-cache.h"
#include "hashmap.h"
#include <assert.h>
#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

struct CacheNode {
	int x;
	int z;
	void *value;
	struct CacheNode *next;
	struct CacheNode *prev;
};

struct LRUCache {
	struct CacheNode *head;
	struct CacheNode *tail;
	struct CacheNode *free_list;
	HASHMAP *hashmap;
};

LRUCACHE *lru_cache_create(size_t capacity) {
	assert(capacity > 1 && "Cache capacity cannot be less then 1");

	HASHMAP *hashmap = hashmap_create(capacity * 2);
	if (hashmap == NULL) {
		return NULL;
	}

	size_t blockSize = sizeof(LRUCACHE) + (capacity * sizeof(struct CacheNode));
	char *block = malloc(blockSize);
	if (block == NULL) {
		hashmap_destroy(hashmap);
		hashmap = NULL;
		return NULL;
	}

	memset(block, 0, blockSize);

	LRUCACHE *cache = (LRUCACHE *)block;

	struct CacheNode *prev = NULL;
	for (size_t i = 0; i < capacity; i++) {
		struct CacheNode *node = &((struct CacheNode *)(block + sizeof(LRUCACHE)))[i];

		if (prev != NULL) {
			prev->next = node;
		}

		node->prev = prev;
		node->next = NULL;

		prev = node;
	}

	cache->free_list = ((struct CacheNode *)(block + sizeof(LRUCACHE)));
	cache->hashmap = hashmap;

	return cache;
}

void lru_cache_destroy(LRUCACHE *cache) {
	assert(cache != NULL);

	// TODO map any remaining stored values to the cleanup function

	hashmap_destroy(cache->hashmap);
	cache->hashmap = NULL;

	free(cache);
}

/** Removes a node from the cache list */
static inline void lru_cache_remove_from_list(LRUCACHE *cache, struct CacheNode *node) {
	assert(cache != NULL && node != NULL);
	if (cache->tail == node) {
		cache->tail = node->prev;
	}

	if (node->prev != NULL) {
		node->prev->next = node->next;
	}

	if (node->next != NULL) {
		node->next->prev = node->prev;
	}

	node->prev = NULL;
	node->next = NULL;
}

/** Moves a detached node to the head */
static inline void lru_cache_move_to_head(LRUCACHE *cache, struct CacheNode *node) {
	assert(cache != NULL );
	assert(node != NULL);
	assert(node->prev == NULL);
	assert(node->next == NULL);

	node->next = cache->head;
	if (cache->head != NULL) {
		cache->head->prev = node;
	}

	cache->head = node;

	if (cache->tail == NULL) {
		cache->tail = node;
	}
}

void lru_cache_put(LRUCACHE *cache, int x, int z, void *value) {
	assert(cache != NULL);
	assert(value != NULL);

	struct CacheNode *node = hashmap_get(cache->hashmap, x, z);
	if (node != NULL) {
		// TODO consider aborting if the value is different! Otherwise at risk of memory leaks!
		node->value = value;

		lru_cache_remove_from_list(cache, node);
		lru_cache_move_to_head(cache, node);
		return;
	}

	if (cache->free_list != NULL) {
		node = cache->free_list;
		cache->free_list = node->next;

		node->x = 0;
		node->z = 0;
		node->value = NULL;
		node->prev = NULL;
		node->next = NULL;
	} else {
		//
		// Eviction time!
		//

		node = cache->tail;
		hashmap_remove(cache->hashmap, node->x, node->z);
		lru_cache_remove_from_list(cache, node);

		node->x = 0;
		node->z = 0;
		node->value = NULL; // In the future we will call a cleanup function before overwriting this.
		node->prev = NULL;
		node->next = NULL;
	}

	//
	// Setup node for new value
	//

	node->x = x;
	node->z = z;
	node->value = value;

	hashmap_insert(cache->hashmap, x, z, node);
	lru_cache_remove_from_list(cache, node);
	lru_cache_move_to_head(cache, node);
}

void *lru_cache_get(LRUCACHE *cache, int x, int z) {
	assert(cache != NULL);

	struct CacheNode *node = hashmap_get(cache->hashmap, x, z);
	if(node != NULL) {
		lru_cache_remove_from_list(cache, node);
		lru_cache_move_to_head(cache, node);

		return node->value;
	}

	return NULL;
}
