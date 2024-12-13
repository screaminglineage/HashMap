# HashMap

A Hashmap implemented in C using macros to mimic a generic data structure. Simply include hashmap.h and use the functions.

```cpp
hashmap_entry(HashMap *, KeyType, ValueType **);
hashmap_insert(HashMap *, KeyType, ValueType);
hashmap_update(HashMap *, KeyType, ValueType);
hashmap_get(HashMap *, KeyType, ValueType);
hashmap_remove(HashMap *, KeyType);
hashmap_items(HashMap *, Function, ...);
```

The core HashMap structure as well as a HashEntry structure (key, value and occupied) needs to be defined, along with a hash function and an equality function.
For example a hashmap with string for KeyType and int for ValueType may be defined as, 

```c
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
```


## Building and Running

```console
$ gcc main.c -o main
$ ./main text.txt
$ ./main text_long.txt
```

## Example

Expanding on the example structure mentioned above. The entire program can be found in [example.c](https://github.com/screaminglineage/HashMap/blob/main/example.c).

```c
// djb2 hash algorithm from http://www.cse.yorku.ca/~oz/hash.html
size_t djb2(const char *str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

int main() {
    HashMap hm = {0};
    hm.eq_fn = strcmp;
    hm.hash_fn = djb2;

    // Insert and Update
    int *v = NULL;
    hashmap_entry(&hm, "hello", &v);
    *v = 123;
    hashmap_entry(&hm, "world", &v);
    *v = 234;
    hashmap_insert(&hm, "a", 212);
    hashmap_insert(&hm, "b", 239);
    hashmap_update(&hm, "b", 190);

    // Get
    int *get = NULL;
    hashmap_get(&hm, "b", &get);
    if (get != NULL) 
        printf("Got: %d\n", *get);
    else
        printf("Key `b` wasnt found\n");

    // Remove
    hashmap_remove(&hm, "b");
    printf("Removed b\n");

    hashmap_get(&hm, "b", &get);
    if (get != NULL)
        printf("Got: %d\n", *get);
    else
        printf("Key `b` wasnt found\n");

    // Iteration
    hashmap_items(&hm, printf, "(%s: %d)\n");

    return 0;
}
```

