#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_INIT_CAP 256

/*
#define hashmap_insert(map, k, v)                                               \
do {                                                                            \
    if (hashmap_get((map), (k), NULL)) return;                                  \
    assert((map).size < (map)->capacity && "TODO: hasmap doesnt auto-grow");     \
    size_t i = hash((k));                                                       \
    while((map).entries[i].occupied) i = (i + 1) % (map).capacity;              \
    (map)->entries[i] = (HashEntry){(k), (v), true};                              \
    (map)->size += 1;                                                            \
} while (0)
*/

typedef int (*hashmap_eq_fn)(const char *a, const char *b);
typedef size_t (*hashmap_hash_fn)(const char *a);


#define hashmap_entry(map, k, v)\
do {\
    assert((map)->size < (map)->capacity && "TODO: hashmap doesnt auto-grow");\
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


#define hashmap_items(map, fn, ...)\
do {\
    for (size_t i = 0; i < (map)->capacity; i++) {\
        if ((map)->entries[i].occupied) {\
            fn(__VA_ARGS__, (map)->entries[i].key, (map)->entries[i].value);\
        }\
    }\
} while (0);\

#endif // !HASHMAP_H
