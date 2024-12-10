#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HASHMAP_INIT_CAP 128

// Get an "entry" into a particular value by key.
// The value is returned by pointer.
// NOTE: The pointer set by this function is temporary and 
// should not be used after another insert into the hashmap.
// If required, the value should be copied to the user's scope.
#define hashmap_entry(map, k, v)\
do {\
    if ((map)->size >= (map)->capacity) {\
        (map)->capacity = (map)->capacity == 0? HASHMAP_INIT_CAP: (map)->capacity * 2;\
        void *mem = calloc((map)->capacity, sizeof(HashEntry));\
        memcpy(mem, (map)->entries, (map)->size*sizeof(HashEntry));\
        void *tmp = (map)->entries;\
        (map)->entries = mem;\
        free(tmp);\
    }\
    size_t i = (map)->hash_fn((k)) % (map)->capacity;\
    size_t j = (i == 0)? (map)->capacity - 1: i - 1;\
    while (i != j && (map)->entries[i].occupied && (map)->eq_fn((map)->entries[i].key, (k)) != 0) \
        i = (i+1)%(map)->capacity;\
    if (!(map)->entries[i].occupied) {\
        (map)->entries[i].key = (k);\
        (map)->entries[i].occupied = true;\
        (map)->size++;\
    }\
    *(v) = &(map)->entries[i].value;\
} while (0);\


// call a function (such as fprintf) on each element of the hashmap
#define hashmap_items(map, fn, ...)\
do {\
    for (size_t i = 0; i < (map)->capacity; i++) {\
        if ((map)->entries[i].occupied) {\
            fn(__VA_ARGS__, (map)->entries[i].key, (map)->entries[i].value);\
        }\
    }\
} while (0);\

#endif // !HASHMAP_H
