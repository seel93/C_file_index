#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "trie.h"
#include "list.h"
#include "printing.h"

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

static inline int cmp_ints(void *a, void *b) {
    return *((int *) a) - *((int *) b);
}


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
    node->value = value;
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
        }
        iter = iter->children[ASCII_TO_IDX(tolower(key[i]))];
    }
    iter->key = key;
    iter->end_of_word = true;

    // Check if node has any indexes for the key
    if (iter->value == NULL) {
        list_t *list = list_create(cmp_ints);
        iter->value = list;
        list_addlast(iter->value, value);
    } else {
        list_addlast(iter->value, value);
    }

    return 0;
    error:
    return -1;
}

static node_t *traverse_trie(node_t *node, char *key){
    int alphabetical_index;
    int query_length = strlen(key);

    for (int key_level = 0; key_level < query_length; key_level++) {
        alphabetical_index = CHAR_TO_INDEX(key[key_level]);
        if (!node->children[alphabetical_index]) {
            return NULL;
        }
        node = node->children[alphabetical_index];
    }
    return node;
}


// kanskje en boolean her for å avgjøre om det er find eller autocomplete
list_t *trie_find(trie_t *trie, char *key, bool is_autocomplete) {
    // Initialize variables:
    int query_length = strlen(key);
    int alphabetical_index;
    struct node *node = trie->root;


    node = traverse_trie(node, key);

    // Tree traversal:
    //for (int key_level = 0; key_level < query_length; key_level++) {
    //    alphabetical_index = CHAR_TO_INDEX(key[key_level]);
    //    if (!node->children[alphabetical_index]) {
    //        return NULL;
    //    }
    //    node = node->children[alphabetical_index];
    //}


    if (node != NULL && node->end_of_word) {
        return node->value;
    }
    //else if (query_length > 2 && is_autocomplete) {
    //    // return remaining keys from the current level of the tree:
    //    list_t *suggested_words = list_create(cmp_ints);
    //    for (int i = 0; i < TRIE_RADIX; ++i) {
    //        if (node->children[i] != NULL && node->children[i]->key != NULL) {
    //            list_addlast(suggested_words, node->children[i]->key);
    //        }
    //    }
    //    return suggested_words;
    //}
    else {
        return NULL;
    }
}


list_t *trie_find_autcomplete(trie_t *trie, char *key, size_t size) {
    int alphabetical_index;
    struct node *node = trie->root;
    int query_length = strlen(key);

    node = traverse_trie(node, key);

    // Tree traversal:
    //for (int key_level = 0; key_level < query_length; key_level++) {
    //    alphabetical_index = CHAR_TO_INDEX(key[key_level]);
    //    if (!node->children[alphabetical_index]) {
    //        return NULL;
    //    }
    //    node = node->children[alphabetical_index];
    //}

    list_t *suggested_words = list_create(cmp_ints);
    int i = 0;
    while (i < TRIE_RADIX) {
        if (node->children[i] != NULL) {
            if (node->children[i]->key != NULL) {
                char *word = node->children[i]->key;
                list_addlast(suggested_words,  word);
            } else {
                node = node->children[i];
            }
        }
        i++;
    }
    return suggested_words;
}
