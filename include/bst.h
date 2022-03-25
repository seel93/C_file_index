//
// Created by oystein on 24.03.2022.
//

#ifndef BST_H
#define BST_H

typedef int (*cmpfunc_t)(void *, void *);

typedef struct bst bst_t;

// Create an empty binary search tree
bst_t *bst_create(cmpfunc_t cmp);

// Destroy the binary search tree
void bst_destroy(bst_t *bst);

// Insert an item into the bst based on its key
int bst_insert(bst_t *bst, void *key, void *val);

// Search for a key in the bst and return its key
void *bst_search(bst_t *bst, void *key);


#endif //PRECODE_BST_H
