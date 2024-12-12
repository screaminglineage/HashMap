#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HASHMAP_INIT_CAP 256

// TODO: rehash existing keys when hashmap grows
// TODO: check if hashmap_insert and hashmap_update work correctly
// TODO: make hashmap_grow which autogrows the hashmap into a separate macro

// Get a pointer into a particular value by key.
// The key is inserted if it doesnt exist
// NOTE: The pointer set by this function is temporary and 
// should not be used after another insert into the hashmap.
// If required, the value should be copied to the user's scope.
#define hashmap_entry(map, k, v)\
do {\
    if ((map)->size >= (map)->capacity) {\
        (map)->capacity = (map)->capacity == 0? HASHMAP_INIT_CAP: (map)->capacity * 2;\
        void *mem = calloc((map)->capacity, sizeof(HashEntry));\
        if ((map)->capacity > HASHMAP_INIT_CAP) memcpy(mem, (map)->entries, (map)->size*sizeof(HashEntry));\
        void *tmp = (map)->entries;\
        (map)->entries = mem;\
        free(tmp);\
    }\
    size_t i = (map)->hash_fn((k)) % (map)->capacity;\
    while ((map)->entries[i].occupied && (map)->eq_fn((map)->entries[i].key, (k)) != 0) \
        i = (i+1)%(map)->capacity;\
    if (!(map)->entries[i].occupied) {\
        (map)->entries[i].key = (k);\
        (map)->entries[i].occupied = true;\
        (map)->size++;\
    }\
    *(v) = &(map)->entries[i].value;\
} while (0);\


// Insert a key-value pair into the hashmap
// Does nothing if the key already exists, use hashmap_entry to update an existing key
#define hashmap_insert(map, k, v)\
do {\
    if ((map)->size >= (map)->capacity) {\
        (map)->capacity = (map)->capacity == 0? HASHMAP_INIT_CAP: (map)->capacity * 2;\
        void *mem = calloc((map)->capacity, sizeof(HashEntry));\
        if ((map)->capacity > HASHMAP_INIT_CAP) memcpy(mem, (map)->entries, (map)->size*sizeof(HashEntry));\
        void *tmp = (map)->entries;\
        (map)->entries = mem;\
        free(tmp);\
    }\
    size_t i = (map)->hash_fn((k)) % (map)->capacity;\
    while ((map)->entries[i].occupied && (map)->eq_fn((map)->entries[i].key, (k)) != 0) \
        i = (i+1)%(map)->capacity;\
    if (!(map)->entries[i].occupied) {\
        (map)->entries[i].key = (k);\
        (map)->entries[i].value = (v);\
        (map)->entries[i].occupied = true;\
        (map)->size++;\
    }\
} while (0);\


// Update the value of an already-existing key in the hashmap
// Does nothing if the key doesnt exist
#define hashmap_update(map, k, v)\
do {\
    if ((map)->capacity == 0) break;\
    size_t i = (map)->hash_fn((k)) % (map)->capacity;\
    size_t n = 0;\
    while (n < (map)->capacity && (map)->entries[i].occupied && (map)->eq_fn((map)->entries[i].key, (k)) != 0) {\
        i = (i+1)%(map)->capacity;\
        n++;\
    }\
    if (n < (map)->capacity && (map)->entries[i].occupied)\
        (map)->entries[i].value = (v);\
} while (0);\


// Get the value for the corresponding key from the hashmap
// Value is passed by pointer and is set to NULL for a non-existant key
// NOTE: Similarly to hashmap_entry, the pointer to the value shouldnt
// be used after another insert into the hashmap
#define hashmap_get(map, k, v)\
do {\
    if ((map)->capacity == 0) break;\
    size_t i = (map)->hash_fn((k)) % (map)->capacity;\
    size_t n = 0;\
    while (n < (map)->capacity && (map)->entries[i].occupied && (map)->eq_fn((map)->entries[i].key, (k)) != 0) {\
        i = (i+1)%(map)->capacity;\
        n++;\
    }\
    *(v) = (n < (map)->capacity && (map)->entries[i].occupied)? &(map)->entries[i].value: NULL;\
} while (0);\


// TODO: add another remove operation that copies the removed value back to the caller
// The only issue with such an operation will be if the value is a pointer to some other
// data structure which cannot be simply memcpy'd unlike primitive types


// Remove a key from the hashmap
// Calling remove on a non-existant key does nothing
#define hashmap_remove(map, k)\
do {\
    if ((map)->capacity == 0) break;\
    size_t i = (map)->hash_fn((k)) % (map)->capacity;\
    size_t n = 0;\
    while (n < (map)->capacity && (map)->entries[i].occupied && (map)->eq_fn((map)->entries[i].key, (k)) != 0) {\
        i = (i+1)%(map)->capacity;\
        n++;\
    }\
    if (n < (map)->capacity && (map)->entries[i].occupied) {\
        (map)->entries[i].occupied = false;\
        (map)->size--;\
    }\
} while (0);\



// Call a function (such as fprintf) on each element of the hashmap
// The key and value are also passed into the function
#define hashmap_items(map, fn, ...)\
do {\
    for (size_t i = 0; i < (map)->capacity; i++) {\
        if ((map)->entries[i].occupied) {\
            fn(__VA_ARGS__, (map)->entries[i].key, (map)->entries[i].value);\
        }\
    }\
} while (0);\

#endif // !HASHMAP_H
