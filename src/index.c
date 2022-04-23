
#include <string.h>

#include "index.h"
#include "map.h"
#include "printing.h"
#include "trie.h"
#include "common.h"
#include "list.h"


/*
 * you might need a struct for keeping track of documents
 */

/*
 * Implement your index here.
 */
struct index
{
    trie_t *trie;
    list_t *document_list;

};

/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result
{
    int location;
    int len;
    /*
     * TODO implement additional fields if necessary:
     * location vil bare gi resultat for ett ord
     * det funker ikke dersom man skal kunne søke på setninger!
     */
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
    index_t *index = malloc(sizeof (index));
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

    int word_index = 0;
    while (list_hasnext(it)){
        trie_insert(idx->trie, list_next(it), &word_index);
        word_index++;
    }
}


/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query)
{
    void *res = trie_find(idx->trie, query);
    DEBUG_PRINT("%s\n", res);
    search_result_t *result = res;
    return res;
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
