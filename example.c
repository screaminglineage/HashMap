#include <stdio.h>
#include "hashmap.h"

typedef int (*hashmap_eq_fn)(const char *a, const char *b);
typedef size_t (*hashmap_hash_fn)(const char *a);

typedef struct {
    const char *key;
    int value;
    bool occupied;
} HashEntry;

typedef struct {
    HashEntry *entries;
    size_t size;
    size_t capacity;
    hashmap_eq_fn eq_fn;
    hashmap_hash_fn hash_fn;
} HashMap;


// djb2 hash algorithm from http://www.cse.yorku.ca/~oz/hash.html
size_t hash_str(const char *str) {
    size_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


// Alternate (verbose) way of creating a hashmap
HashMap example_init() {
    return (HashMap) {
        .entries = calloc(HASHMAP_INIT_CAP, sizeof(HashEntry)),
        .size = 0,
        .capacity = HASHMAP_INIT_CAP,
        .eq_fn = strcmp,
        .hash_fn = hash_str
    };
}

int main() {
    HashMap hm = {0};
    hm.eq_fn = strcmp;
    hm.hash_fn = hash_str;

    // Insert
    const char *k = "hello";
    int *v = NULL;
    hashmap_entry(&hm, k, &v);
    *v = 123;

    // Get
    int *v2 = NULL;
    hashmap_entry(&hm, k, &v2);
    if (v2 != NULL)
        printf("Value: %d\n", *v2);

    return 0;
}

