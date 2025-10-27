/**
 * Static open address hashmap implementation for use with a X, Z grid of chunks
 * for use within a LRU cache.
 */

#include "hashmap.h"
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

#define HASHMAP_FREE	  0
#define HASHMAP_TOMBSTONE 1
#define HASHMAP_OCCUPIED  2

struct HashmapEntry {
	int x, z;
	void *value;
};

struct Hashmap {
	struct HashmapEntry *data;
	uint8_t *state;
	size_t capacity;
	size_t size;
};

/**
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
uint64_t fnv1a_hash(const void *data, size_t len) {
	const unsigned char *bytes = (const unsigned char *)data;
	uint64_t hash = 1469598103934665603ULL;	 // offset basis
	const uint64_t prime = 1099511628211ULL; // FNV prime

	for (size_t i = 0; i < len; i++) {
		hash ^= (uint64_t)bytes[i];
		hash *= prime;
	}

	return hash;
}

static inline uint64_t fnv1a_hash_coords(int x, int z, size_t hashmap_capacity) {
	struct {
		int x;
		int z;
	} key = {x, z};

	uint64_t hash = fnv1a_hash(&key, sizeof(key));
	return hash % hashmap_capacity;
}

HASHMAP *hashmap_create(size_t capacity) {
	assert(capacity > 0);

	size_t block_size = sizeof(HASHMAP) + capacity * sizeof(struct HashmapEntry) +
		capacity * sizeof(uint8_t);

	char *block = malloc(block_size);
	if (block == NULL) {
		return NULL;
	}

	memset(block, 0, block_size);

	HASHMAP *hashmap = (HASHMAP *)block;
	hashmap->data = (struct HashmapEntry *)(block + sizeof(HASHMAP));
	hashmap->state = (uint8_t *)(block + sizeof(HASHMAP) + sizeof(struct HashmapEntry) * capacity);
	hashmap->capacity = capacity;

	return hashmap;
}

void hashmap_destroy(HASHMAP *hashmap) {
	assert(hashmap != NULL);
	free(hashmap);
}

bool hashmap_insert(HASHMAP *hashmap, int x, int z, void *value) {
	assert(hashmap != NULL && value != NULL);

	if (hashmap->size == hashmap->capacity) {
		return false;
	}

	uint64_t preferred_index = fnv1a_hash_coords(x, z, hashmap->capacity);
	uint64_t first_tombstone = UINT64_MAX;

	uint64_t index = preferred_index;

	do {
		uint8_t state = hashmap->state[index];

		if (state == HASHMAP_OCCUPIED) {
			struct HashmapEntry *entry = &hashmap->data[index];

			if (entry->x == x && entry->z == z) {
				entry->value = value;
				return true;
			}
		} else if (state == HASHMAP_TOMBSTONE && first_tombstone == UINT64_MAX) {
			first_tombstone = index;
		} else if (state == HASHMAP_FREE) {
			break;
		}

		index = (index + 1) % hashmap->capacity;
	} while (index != preferred_index);

	uint64_t target = (first_tombstone != UINT64_MAX) ? first_tombstone : index;

	hashmap->state[target] = HASHMAP_OCCUPIED;
	hashmap->data[target].x = x;
	hashmap->data[target].z = z;
	hashmap->data[target].value = value;

	hashmap->size++;

	return true;
}

void *hashmap_remove(HASHMAP *hashmap, int x, int z) {
	assert(hashmap != NULL);

	uint64_t preferred_index = fnv1a_hash_coords(x, z, hashmap->capacity);

	uint64_t index = preferred_index;
	do {
		uint8_t state = hashmap->state[index];
		if (state == HASHMAP_OCCUPIED) {
			struct HashmapEntry *entry = &hashmap->data[index];
			if (entry->x == x && entry->z == z) {
				void *value = entry->value;

				uint64_t next_index = (index + 1) % hashmap->capacity;
				hashmap->state[index] = hashmap->state[next_index] == HASHMAP_FREE
					? HASHMAP_FREE
					: HASHMAP_TOMBSTONE;
				hashmap->size--;

				entry->x = 0;
				entry->z = 0;
				entry->value = NULL;

				return value;
			}
		} else if (state == HASHMAP_FREE) {
			break; // Key not found!
		}

		index = (index + 1) % hashmap->capacity;
	} while (index != preferred_index);

	return NULL;
}

void *hashmap_get(HASHMAP *hashmap, int x, int z) {
	assert(hashmap != NULL);

	uint64_t preferred_index = fnv1a_hash_coords(x, z, hashmap->capacity);

	uint64_t index = preferred_index;
	do {
		uint8_t state = hashmap->state[index];
		if (state == HASHMAP_OCCUPIED) {
			struct HashmapEntry *entry = &hashmap->data[index];
			if (entry->x == x && entry->z == z) {
				return entry->value;
			}
		} else if (state == HASHMAP_FREE) {
			break; // Key not found!
		}

		index = (index + 1) % hashmap->capacity;
	} while (index != preferred_index);

	return NULL;
}

size_t hashmap_size(HASHMAP *hashmap) {
	assert(hashmap != NULL);
	return hashmap->size;
}
