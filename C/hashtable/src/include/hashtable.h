#ifndef __int_HASHTABLE
#define __int_HASHTABLE 1

#include <stdlib.h>

typedef struct
{
    const char *key;
    int value;
} int_ht_entry_t;

typedef struct
{
    int_ht_entry_t *entries;
    size_t capacity;
    size_t length;
} ht_int_t;

ht_int_t ht_int_create(void);
void ht_int_destroy(ht_int_t *ht);
bool ht_int_has(ht_int_t ht, const char *key);
bool ht_int_set(ht_int_t ht, const char *key, int value);
int ht_int_get(ht_int_t ht, const char *key);
size_t ht_int_length(ht_int_t ht, const char *key);

#ifdef int_HASHTABLE_IMPLEMENTATION

#endif // int_HASHTABLE_IMPLEMENTATION

#endif // __int_HASHTABLE