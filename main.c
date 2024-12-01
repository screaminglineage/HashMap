#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

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


#define hashmap_entry(map, k, v)\
do {\
    size_t i = (map)->hash_fn((k)) % (map)->capacity;\
    size_t j = (i == 0)? (map)->capacity - 1: i - 1;\
    while (i != j && (map)->entries[i].occupied && (map)->eq_fn((map)->entries[i].key, (k)) != 0) \
        i = (i+1)%(map)->capacity;\
    assert((map)->size < (map)->capacity && "TODO: hashmap doesnt auto-grow");\
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

size_t hash_str(const char *str) {
    size_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

char *read_to_str(char* filepath) {
    FILE* file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* data = malloc(size + 1);
    fread(data, size, 1, file);
    fclose(file);

    data[size] = 0;
    return data;
}

int hash_entry_cmp(const void *a, const void *b) {
    return ((HashEntry *)b)->value - ((HashEntry *)a)->value;
}

int hashmap_example() {
    HashMap hm = {
        .entries = calloc(HASHMAP_INIT_CAP, sizeof(HashEntry)),
        .size = 0,
        .capacity = HASHMAP_INIT_CAP,
        .eq_fn = strcmp,
        .hash_fn = hash_str
    };

    const char *k = "hello";
    int *v = NULL;
    hashmap_entry(&hm, k, &v);
    *v = 123;

    int *v2;
    hashmap_entry(&hm, k, &v2);
    printf("%d\n", *v2);
    return 0;
}

void str_lower(char *str) {
    for (size_t i = 0; str[i]; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] += 32;
        }
    }
}


int main(int argc, char *argv[]) {
    HashMap hm = {
        .entries = calloc(HASHMAP_INIT_CAP, sizeof(HashEntry)),
        .size = 0,
        .capacity = HASHMAP_INIT_CAP,
        .eq_fn = strcmp,
        .hash_fn = hash_str
    };

    if (argc < 1) {
        printf("Not Enought Arguments\n");
        printf("usage: <program> <text-file>\n");
        return 1;
    }

    char *program_name = argv[0];
    if (argc < 2) {
        printf("usage: %s <text-file>\n", program_name);
        printf("<text-file> - lists out word frequencies based on the file, only supports one line of text for now");
        return 1;
    }

    char *text = read_to_str(argv[1]);
    text[strcspn(text, "\r\n")] = 0;
    char *word = strtok(text, " ");
    while (word) {
        int *count;
        str_lower(word);
        hashmap_entry(&hm, word, &count);
        *count += 1;
        word = strtok(NULL, " ");
    }

    printf("Size: %zu\n", hm.size);
    printf("HashMap Entries:\n");
    hashmap_items(&hm, printf, "Key: `%s`, Value: %d\n");
    printf("--------------------\n\n");

    HashEntry entries[hm.size];
    for (size_t i = 0, j = 0; i < hm.capacity; i++) {
        if (hm.entries[i].occupied) entries[j++] = hm.entries[i];
    }

    qsort(entries, hm.size, sizeof(HashEntry), hash_entry_cmp);
    for (size_t i = 0; i < hm.size; i++) {
        printf("Word: `%s`, Count: %d\n", entries[i].key, entries[i].value);
    }

    printf("\n");
    return 0;

}
