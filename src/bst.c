//
// Created by oystein on 24.03.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"
#include "printing.h"

typedef struct node node_t;
struct node
{
    node_t *left;
    node_t *right;
    void *key;
    void *val;
};


struct bst
{
    node_t *root;
    cmpfunc_t cmp;
};


static node_t *newnode(void *key, void *val)
{
    node_t      *node;

    node = (node_t*)malloc(sizeof(node_t));
    if (node == NULL)
        goto error;

    node->key = key;
    node->val = val;
    node->left = NULL;
    node->right = NULL;

    return node;

    error:
    return NULL;
}


// Create an empty binary search tree
bst_t *bst_create(cmpfunc_t cmp)
{
    bst_t *bst = malloc(sizeof(bst_t));
    if (bst == NULL)
    {
        fprintf(stderr, "Could not allocate memory for new bst\n");
        return NULL;
    }

    bst->root = NULL;
    bst->cmp = cmp;

    return bst;
}

static void _bst_destroy(node_t *root)
{
    if (root == NULL)
    {
        return;
    }
    _bst_destroy(root->left);
    _bst_destroy(root->right);
    free(root);
}


// Destroy the binary search tree
void bst_destroy(bst_t *bst)
{
    if (bst->root != NULL)
    {
        _bst_destroy(bst->root);
    }
    free(bst);
}


static node_t *bst_insert_node(bst_t *bst, node_t *node, node_t *new, int *error)
{
    if (node == NULL)
    {
        return new;
    }

    int cmpres = bst->cmp(node->key, new->key);
    if (cmpres == 0)
    {
        *error = -1;
    }
    else if (cmpres > 0)
    {
        node->left = bst_insert_node(bst, node->left, new, error);
    }
    else
    {
        node->right = bst_insert_node(bst, node->right, new, error);
    }

    return node;
}


int bst_insert(bst_t *bst, void *key, void *val)
{

    DEBUG_PRINT("%zu \n", strlen(key));
    DEBUG_PRINT("%zu \n", strlen(val));

    int err = 0;
    node_t *n = newnode(key, val);
    if (n == NULL)
    {
        goto error;
    }

    bst->root = bst_insert_node(bst, bst->root, n, &err);
    if (err == -1)
    {
        // fprintf(stderr, "Key already in bst\n");
        goto error;
    }

    return 0;

    error:
    if (n != NULL)
    {
        free(n);
    }

    return -1;
}

static void *_bst_search(node_t *root, void *key, cmpfunc_t cmp)
{
    if (root == NULL)
    {
        return NULL;
    }

    if (cmp(root->key, key) == 0)
    {
        return root->val;
    }
    else if (cmp(root->key, key) > 0)
    {
        return _bst_search(root->left, key, cmp);
    }
    else
    {
        return _bst_search(root->right, key, cmp);
    }

}

// Search for a key in the bst and return its key
void *bst_search(bst_t *bst, void *key)
{
    return _bst_search(bst->root, key, bst->cmp);
}


