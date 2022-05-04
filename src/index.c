
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
    map_t *document_map;
    list_t *document_list;
};


/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result {
    map_t *search_result_map;
    list_t *document_list;
    list_t *document_words;
    list_iter_t *document_iterator;
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
    index->document_map = map_create(cmp_strs, djb2);
    index->document_list = list_create(cmp_strs);
    return index;
}


/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index) {
    map_destroy(index->map, NULL, NULL);
    list_destroy(index->document_list);
    free(index);
}

/*
 * Removing non-aplhabetic symbols for char
 */
void removeChar(char *str, char garbage) {
    if (!isspace(garbage)) {
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
    list_t *words_from_document = list_create(cmp_strs);

    int word_index = 0;
    while (list_hasnext(it)) {
        char *key = list_next(it);
        bool valid_key = false;
        for (int i = 0; key[i] != '\0'; i++) {
            if (!isalpha(key[i])) {
                removeChar(key, key[i]);
            } else {
                valid_key = true;
            }
        }
        if (valid_key) {
            int *p = malloc(sizeof(int));
            *p = word_index;
            trie_insert(trie, key, p);
            list_addlast(words_from_document ,key);
            word_index++;
        }
    }
    map_put(idx->map, document_name, trie);
    map_put(idx->document_map, document_name, words_from_document);
}


/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query) {
    list_iter_t *document_iterator = list_createiter(idx->document_list);
    search_result_t *search_result_object = malloc(sizeof(search_result_t));

    search_result_object->search_result_map = map_create(cmp_strs, djb2);
    search_result_object->document_list = list_create(cmp_strs);

    while (list_hasnext(document_iterator)) {
        char *document_name = list_next(document_iterator);
        search_result_object->document_words = map_get(idx->document_map, document_name);
        trie_t *trie = map_get(idx->map, document_name);
        list_t *result_set = trie_find(trie, query);
        list_addlast(search_result_object->document_list, document_name);
        if (result_set != NULL) {
            list_iter_t *iter = list_createiter(result_set);
            list_t *search_result_list = list_create(cmp_ints);
            while (list_hasnext(iter)) {
                int *elem = list_next(iter);
                search_hit_t *hit = malloc(sizeof(search_hit_t));
                hit->len = strlen(query);
                hit->location = *(int *) elem;
                list_addlast(search_result_list, hit);
            }
            map_put(search_result_object->search_result_map, document_name, search_result_list);
            list_destroy(result_set);
        }
    }
    search_result_object->document_iterator = list_createiter(search_result_object->document_list);
    DEBUG_PRINT("result complete with map object \n");
    return search_result_object;
}


/*
 * Autocomplete searches the given trie for a word containing input.
 * The input string is NULL terminated and contains size letters (excluding null termination).
 * The output string MUST be null terminated.
 */
char *autocomplete(index_t *idx, char *input, size_t size) {
    list_iter_t *it = list_createiter(idx->document_list);
    while (list_hasnext(it)) {
        trie_t *trie = map_get(idx->map, list_next(it));
        list_t *result_set = trie_find_autcomplete(trie, input, size);
        if (result_set != NULL) {
            list_iter_t *iter = list_createiter(result_set);
            if (list_hasnext(iter)) {
                return (char *) list_next(iter);
            }
        }
    }
    return "";
}


/* 
 * Return the content of the current document.
 * Subsequent calls to this function should return the next document.
 * This function should only be called once for each document.
 * This function should return NULL if there are no more documents.
 */
char **result_get_content(search_result_t *res) {

    if(list_hasnext(res->document_iterator)){
        list_iter_t *results_for_ui_iterator = list_createiter(res->document_words);

        int arr_size = list_size(res->document_words);
        char **arr;
        arr = calloc(list_size(res->document_words), sizeof (char *));


        for (int i = 0; i < arr_size; ++i) {
            if(list_hasnext(results_for_ui_iterator)){
                arr[i] = list_next(results_for_ui_iterator);
            }
        }
        return arr;
    }
    return NULL;
}


/*
 * Get the length of the current document.
 * Subsequent calls should return the length of the same document.
 */
int result_get_content_length(search_result_t *res) {
    if(list_hasnext(res->document_iterator)){
        list_t *result_list = map_get(res->search_result_map, list_next(res->document_iterator));
        return list_size(result_list);
    }
    return NULL;
}


/*
 * Get the next result_list from the current query.
 * The result_list should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res) {
    if(list_hasnext(res->document_iterator)){
        list_t *result_list = map_get(res->search_result_map, list_next(res->document_iterator));
        list_sort(result_list);
        list_iter_t *result_list_iterator = list_createiter(result_list);
        if(list_hasnext(result_list_iterator)){
            search_hit_t *hit = list_next(result_list_iterator);
            return hit;
        }else {
            return NULL;
        }
    }
    return NULL;
}
