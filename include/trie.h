#ifndef TRIE_H
#define TRIE_H

#include "common.h"
#include "list.h"
#include <stdbool.h>

typedef struct trie trie_t;

/*
 * Creates an empty trie
 */
trie_t *trie_create();


/*
 * Destroys the given trie and its nodes
 */
void trie_destroy(trie_t *trie);


/*
 * Inserts a key and value into the trie
 */
int trie_insert(trie_t *trie, char *key, void *value);


/*
 * Finds a word in the trie.
 */
list_t *trie_find(trie_t *trie, char *prefix);

/*
 * Finds suggested words closest to the input provided in the current level of the trie
 */
list_t *trie_find_autcomplete(trie_t *trie, char *prefix, size_t size);

#endif

