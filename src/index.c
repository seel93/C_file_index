
#include <string.h>

#include "index.h"
#include "map.h"
#include "printing.h"
#include "trie.h"
#include "bst.h"
#include "common.h"
#include "list.h"


/*
 * you might need a struct for keeping track of documents
 */



/*
 * Implement your index here.
 */
typedef struct bst bst_t;
struct index
{
    //bst_t *bst;
    trie_t *trie;
    list_t *document_list;
    // TODO implement
};

/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result
{
    int location;
    int len;
    // TODO implement additional fields if necessary
};


static inline int cmp_ints(void *a, void *b)
{
    return *((int *)a) - *((int *)b);
}

/*
 * Compares two strings without case-sensitivity
 */ 
static inline int cmp_strs(void *a, void *b)
{
    return strcasecmp((const char *)a, (const char *)b);
}



/*
 * Creates a new, empty index.
 */
index_t *index_create()
{
    DEBUG_PRINT("Creating index \n");
    index_t *index = malloc(sizeof (index));
    //index->bst = bst_create(cmp_strs);
    index->trie = trie_create();
    index->document_list = list_create(cmp_strs);
    return index;
}


/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index)
{
    //bst_destroy(index->bst);
    trie_destroy(index->trie);
    list_destroy(index->document_list);
    free(index);
}


/*
 * Adds all the words from the given document to the given index.
 * This function is responsible for deallocating the list and the document name after use.
 */
void index_add_document(index_t *idx, char *document_name, list_t *words){
    list_iter_t *it;
    list_addlast(idx->document_list, document_name);
    it = list_createiter(words);

    while (list_hasnext(it)){
        DEBUG_PRINT("%s\n", list_next(it));
        //bst_insert(idx->bst, (void *) hash_string(list_next(iter)), list_next(iter));
        int a = trie_insert(idx->trie, hash_string(list_next(it)), list_next(it));
        if (a == -1){
            DEBUG_PRINT("failed to insert");
        }else{
            DEBUG_PRINT("Trie insert succes");
        }
    }
}


/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query)
{
    //void *res = bst_search(idx->bst, query);
    void *res = trie_find(idx->trie, query);
    DEBUG_PRINT("%p\n", res);
    return NULL;
}


/*
 * Autocomplete searches the given trie for a word containing input.
 * The input string is NULL terminated and contains size letters (excluding null termination).
 * The output string MUST be null terminated.
 */ 
char *autocomplete(index_t *idx, char *input, size_t size)
{
    return NULL;
}


/* 
 * Return the content of the current document.
 * Subsequent calls to this function should return the next document.
 * This function should only be called once for each document.
 * This function should return NULL if there are no more documents.
 */
char **result_get_content(search_result_t *res)
{
    return NULL;
}


/*
 * Get the length of the current document.
 * Subsequent calls should return the length of the same document.
 */
int result_get_content_length(search_result_t *res)
{
    return NULL;
}


/*
 * Get the next result from the current query.
 * The result should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res)
{
    return NULL;
}
