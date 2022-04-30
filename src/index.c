
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "index.h"
#include "printing.h"
#include "trie.h"
#include "common.h"
#include "list.h"
#include "map.h"


/*
 * you might need a struct for keeping track of documents
 */

/*
 * Implement your index here.
 */
struct index {
    map_t *map;
    list_t *document_list;
    //trie_t *trie;
};


/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result {
    // Denne må gjøre om!
    int location;
    int len;
};


static inline int cmp_ints(void *a, void *b) {
    return *((int *) a) - *((int *) b);
}

/*
 * Compares two strings without case-sensitivity
 */
static inline int cmp_strs(void *a, void *b) {
    return strcasecmp((const char *) a, (const char *) b);
}


/*
 * Creates a new, empty index.
 */
index_t *index_create() {
    index_t *index = malloc(sizeof(index));
    index->map = map_create(cmp_strs, djb2);
    index->document_list = list_create(cmp_strs);
    //index->map = trie_create();
    return index;
}


/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index) {
    map_destroy(index->map, NULL, NULL);
    // TODO you might need to iterate over all keys in the map to destroy the trie's as well
    //trie_destroy(index->trie);
    //list_destroy(index->document_list);
    free(index);
}


// TODO add helpermethod for list_next(it)

char *trim_input(char *key) {
    int i, j;
    char trimmed_key[strlen(key)];
    for (i = 0, j = 0; key[i] != '\0'; i++) {
        // TODO:  legg til tolower() her
        if ((key[i] >= 'A' && key[i] <= 'Z') || (key[i] >= 'a' && key[i] <= 'z')) {
            trimmed_key[j] = key[i];
            j++;
        }
    }
    trimmed_key[j] = '\0';
    return trimmed_key;
}

/*
 * Adds all the words from the given document to the given index.
 * This function is responsible for deallocating the list and the document name after use.
 */
void index_add_document(index_t *idx, char *document_name, list_t *words) {
    list_iter_t *it;
    list_addlast(idx->document_list, document_name);
    it = list_createiter(words);
    trie_t *trie = trie_create();

    int word_index = 0;
    void *p;
    while (list_hasnext(it)) {
        p = word_index;
        char *key = list_next(it);
        for (int i = 0; key[i] != '\0'; i++) {
            if (!isalpha(key[i])) {
                trim_input(key);
                break;
            }
        }
        bool valid_key = false;
        for (int i = 0; key[i] != '\0'; ++i) {
            if (!isspace(key[i]) && isalpha(key[i])) {
                valid_key = true;
            }
        }
        if (valid_key) {
            trie_insert(trie, key, p);
            word_index++;
        }
    }
    map_put(idx->map ,document_name, trie);
}


/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query) {
    // TODO: iterate over all keys(documents) and populate search result:
    list_iter_t *it = list_createiter(idx->document_list);
    DEBUG_PRINT("looking for %s \n", query);
    while (list_hasnext(it)){
        DEBUG_PRINT("iterating over documents \n");
        void *res = trie_find(map_get(idx->map, list_next(it)), query);
        DEBUG_PRINT("%s", (char *) res);
    }
    // TODO: create search_result struct showing results for each document
    return NULL;
}


/*
 * Autocomplete searches the given trie for a word containing input.
 * The input string is NULL terminated and contains size letters (excluding null termination).
 * The output string MUST be null terminated.
 */
char *autocomplete(index_t *idx, char *input, size_t size) {
    return NULL;
}


/* 
 * Return the content of the current document.
 * Subsequent calls to this function should return the next document.
 * This function should only be called once for each document.
 * This function should return NULL if there are no more documents.
 */
char **result_get_content(search_result_t *res) {
    return NULL;
}


/*
 * Get the length of the current document.
 * Subsequent calls should return the length of the same document.
 */
int result_get_content_length(search_result_t *res) {
    return NULL;
}


/*
 * Get the next result from the current query.
 * The result should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res) {
    return NULL;
}
