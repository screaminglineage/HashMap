#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
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
size_t djb2_hash(const char *str) {
    size_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int hash_entry_cmp(const void *a, const void *b) {
    return ((HashEntry *)b)->value - ((HashEntry *)a)->value;
}

void str_lower(char *str) {
    for (size_t i = 0; str[i]; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] += 32;
        }
    }
}

typedef struct {
    char *str;
    size_t begin;
    size_t end;
} Tokenizer;


bool next(Tokenizer *tk, char **next) {
    while (tk->str[tk->end] != 0 && (tk->str[tk->end] == ' ' || tk->str[tk->end] == '\n' || tk->str[tk->end] == '\r')) {
        tk->end++;
    }
    if (tk->str[tk->end] == 0) return false;

    tk->begin = tk->end;
    while (tk->str[tk->end] != 0 && (tk->str[tk->end] != ' ' && tk->str[tk->end] != '\n' && tk->str[tk->end] != '\r')) {
        tk->end++;
    }
    if (tk->str[tk->end] == 0 && tk->end - tk->begin == 0) return false;

    *next = &tk->str[tk->begin];
    tk->str[tk->end++] = 0;
    tk->begin = tk->end;
    return true;
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


int main(int argc, char *argv[]) {
    HashMap hm = {0};
    hm.eq_fn = strcmp;
    hm.hash_fn = djb2_hash;

    if (argc < 1) {
        printf("Not Enough Arguments\n");
        printf("usage: <program> <text-file>\n");
        return 1;
    }

    char *program_name = argv[0];
    if (argc < 2) {
        printf("usage: %s <text-file>\n", program_name);
        printf("<text-file> - lists out word frequencies based on the file\n");
        return 1;
    }

    char *text = read_to_str(argv[1]);
    Tokenizer tk = { .str = text };

    char *word = NULL;
    while (next(&tk, &word)) {
        str_lower(word);
        int *count;
        hashmap_entry(&hm, word, &count);
        *count += 1;
    }

    HashEntry entries[hm.size];
    for (size_t i = 0, j = 0; i < hm.capacity; i++) {
        if (hm.entries[i].occupied) entries[j++] = hm.entries[i];
    }

    qsort(entries, hm.size, sizeof(HashEntry), hash_entry_cmp);
    printf("Words sorted by frequency:\n");
    for (size_t i = 0; i < hm.size; i++) {
        printf("Word: `%s`, Count: %d\n", entries[i].key, entries[i].value);
    }
    printf("--------------------\n\n");
    printf("Final Size: %zu, Final Capacity %zu\n", hm.size, hm.capacity);

    printf("\n");
    return 0;

}
