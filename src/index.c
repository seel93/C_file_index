
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "index.h"
#include "printing.h"
#include "trie.h"
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
};


/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result {
    list_t *result_list;
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
    return index;
}


/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index) {
    map_destroy(index->map, NULL, NULL);
    free(index);
}

/*
 * Removing non-aplhabetic symbols for char
 */
void removeChar(char *str, char garbage) {
    if(!isspace(garbage)){
        char *src, *dst;
        for (src = dst = str; *src != '\0'; src++) {
            *dst = *src;
            if (*dst != garbage) dst++;
        }
        *dst = '\0';
    }
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
    while (list_hasnext(it)) {
        char *key = list_next(it);
        bool valid_key = false;
        for (int i = 0; key[i] != '\0'; i++) {
            if (!isalpha(key[i])) {
                removeChar(key, key[i]);
            }else {
                valid_key = true;
            }
        }
        if (valid_key) {
            int *p = malloc(sizeof(int));
            *p = word_index;
            trie_insert(trie, key, p);
            word_index++;
        }
    }
    map_put(idx->map, document_name, trie);
}







/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query) {
    list_iter_t *it = list_createiter(idx->document_list);
    search_result_t *search_result_list = malloc(sizeof(search_result_t));
    search_result_list->result_list = list_create(cmp_ints);
    while (list_hasnext(it)) {
        trie_t *trie = map_get(idx->map, list_next(it));
        list_t *result_set = trie_find(trie, query, false);
        if (result_set != NULL) {
            list_iter_t *iter = list_createiter(result_set);
            while (list_hasnext(iter)) {
                int *elem = list_next(iter);
                search_hit_t *hit = malloc(sizeof(search_hit_t));
                hit->len = strlen(query);
                hit->location = elem;
                list_addlast(search_result_list->result_list, hit);
            }
        }
    }
    DEBUG_PRINT("find complete with %d \n", list_size(search_result_list->result_list));
    return search_result_list;
}


/*
 * Autocomplete searches the given trie for a word containing input.
 * The input string is NULL terminated and contains size letters (excluding null termination).
 * The output string MUST be null terminated.
 */
char *autocomplete(index_t *idx, char *input, size_t size) {
    list_iter_t *it = list_createiter(idx->document_list);
    search_result_t *search_result_list = malloc(sizeof(search_result_t));
    search_result_list->result_list = list_create(cmp_ints);
    while (list_hasnext(it)) {
        trie_t *trie = map_get(idx->map, list_next(it));
        list_t *result_set = trie_find(trie, input, true);
        if (result_set != NULL) {
            list_iter_t *iter = list_createiter(result_set);
            while (list_hasnext(iter)) {
                int *elem = list_next(iter);
                search_hit_t *hit = malloc(sizeof(search_hit_t));
                hit->len = strlen(input);
                hit->location = elem;
                list_addlast(search_result_list->result_list, hit);
            }
        }
    }
    DEBUG_PRINT("find complete with %d \n", list_size(search_result_list->result_list));
    return search_result_list;
}


/* 
 * Return the content of the current document.
 * Subsequent calls to this function should return the next document.
 * This function should only be called once for each document.
 * This function should return NULL if there are no more documents.
 */
char **result_get_content(search_result_t *res) {
    return res->result_list;
}


/*
 * Get the length of the current document.
 * Subsequent calls should return the length of the same document.
 */
int result_get_content_length(search_result_t *res) {
    return list_size(res->result_list);
}


/*
 * Get the next result_list from the current query.
 * The result_list should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res) {
    list_iter_t *iter = list_createiter(res->result_list);
    if (list_hasnext(iter)) {
        return list_next(iter);
    }
    return NULL;
}
