#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "trie.h"
#include "printing.h"
#include "set.h"
#include "common.h"

#define TRIE_RADIX 26
#define ASCII_TO_IDX(c) c - 97
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')


typedef struct node node_t;
struct node {
    char *key;
    void *value;
    bool end_of_word;
    node_t *children[TRIE_RADIX];
};

struct trie {
    node_t *root;
};


static inline int isleaf(node_t *node) {
    // A NULL node is not considered a leaf node
    if (node == NULL) {
        return 0;
    }

    for (int i = 0; i < TRIE_RADIX; i++) {
        if (node->children[i] != NULL) {
            return 0;
        }
    }

    return 1;
}

static node_t *node_create(char *key, void *value) {
    node_t *node = (node_t *) calloc(1, sizeof(node_t));
    if (node == NULL) {
        goto error;
    }

    node->key = key;
    set_t *set = set_create(compare_strings);
    set_add(set, value);
    node->value = (struct set_t *) set;
    node->end_of_word = false;
    return node;

    error:
    return NULL;
}


void node_destroy(node_t *node) {
    free(node);
}

trie_t *trie_create() {

    trie_t *t = (trie_t *) calloc(1, sizeof(trie_t));

    if (t == NULL) {
        goto error;
    }

    t->root = node_create(NULL, NULL);
    return t;

    error:
    return NULL;
}

void _trie_destroy(node_t *node) {
    DEBUG_PRINT("%p", node->key);
    if (isleaf(node)) {
        node_destroy(node);
    } else {
        int i;
        for (i = 0; i < TRIE_RADIX; i++) {
            if (node->children[i] != NULL) {
                _trie_destroy(node->children[i]);
                node->children[i] = NULL;
            }
        }
        node_destroy(node);
    }

    return;
}


void trie_destroy(trie_t *trie) {
    _trie_destroy(trie->root);
    free(trie);
    trie = NULL;
}


int trie_insert(trie_t *trie, char *key, void *value) {
    node_t *iter = trie->root;
    // Only allow alphabet characters
    for (int i = 0; key[i] != '\0'; i++) {
        if (!isalpha(key[i])) {
            goto error;
        }
    }
    // Find the child indices
    for (int i = 0; key[i] != '\0'; i++) {
        // We only use lowercase letters (case insensitive)
        if (iter->children[ASCII_TO_IDX(tolower(key[i]))] == NULL) {
            iter->children[ASCII_TO_IDX(tolower(key[i]))] = node_create(NULL, NULL);
        } else if (iter->children[ASCII_TO_IDX(tolower(key[i]))]->end_of_word == true && isleaf(iter->children[ASCII_TO_IDX(tolower(key[i]))])) {
            DEBUG_PRINT("adding elemnt: %d", (int *) value);
            set_add(iter->children[i]->value, value);
        }
        iter = iter->children[ASCII_TO_IDX(tolower(key[i]))];
    }
    iter->key = key;
    iter->end_of_word = true;
    DEBUG_PRINT("node created with key: %s value: %d \n", key, (int*) value);
    return 0;
    error:
    return -1;
}

char *trie_find(trie_t *trie, char *key) {
    // Initialize variables:
    int query_length = strlen(key);
    int alphabetical_index;
    struct node *node = trie->root;

    // Tree traversal:
    for (int key_level = 0; key_level < query_length; key_level++) {
        alphabetical_index = CHAR_TO_INDEX(key[key_level]);
        if (!node->children[alphabetical_index]) {
            return NULL;
        }
        node = node->children[alphabetical_index];
    }

    if (node != NULL && node->end_of_word) {
        set_iter_t *iter = set_create_iter(node->value);
        while (set_has_next(iter)) {
            DEBUG_PRINT("here is an index: %d \n", (int *) set_next(iter));
        }
        return node->value;
        //node->doc_name
    }
    // return remaining keys from the current level of the tree:
    return NULL;
}
