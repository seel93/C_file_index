#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "index.h"
#include "printing.h"
#include "trie.h"
#include "list.h"
#include "map.h"

/*
 * Struct for keeping track of documents
 */
struct document {
    list_t *document_list;
};

/*
 * Implement your index here.
 */
struct index {
    map_t *index_map;
    map_t *document_map;
};

/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result {
    map_t *search_result_map; // key: search query value: list_t of indexes
    list_t *document_list; // list of documents
    map_t *document_map; // key: document_name value: list_t of all words within the document
    list_iter_t *document_iterator; // keep track of documents
    list_iter_t *current_result_iterator; // keeps track of all search_hit_t
    char *current_document;
};

/*
 * Compares two numerical values
 */
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
    index->index_map = map_create(cmp_strs, djb2);
    index->document_map = map_create(cmp_strs, djb2);
    return index;
}

/*
* Creates a new, empty document connected to the index
*/
document_t *document_create() {
    document_t *document = malloc(sizeof(document));
    document->document_list = list_create(cmp_strs);
    return document;
}

/*
 * Creates a new, empty search result
 */
search_result_t *create_search_result() {
    search_result_t *search_result_object = malloc(sizeof(search_result_t));
    search_result_object->search_result_map = map_create(cmp_strs, djb2);
    search_result_object->document_list = list_create(cmp_strs);
    search_result_object->document_map = map_create(cmp_strs, djb2);
    return search_result_object;
}

/*
 * Destroys the given document
 */
void document_destroy(document_t *document) {
    list_destroy(document->document_list);
    free(document);
}

/*
 * Destroys the given index. Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index) {
    map_destroy(index->index_map, NULL, NULL);
    map_destroy(index->document_map, NULL, NULL);
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
 * Removes non-alphabetic symbols
 */
char *process_key(char *key) {
    bool valid_key = false;
    for (int i = 0; key[i] != '\0'; i++) {
        if (!isalpha(key[i])) {
            removeChar(key, key[i]);
        } else {
            valid_key = true;
        }
    }
    if (valid_key) {
        return key;
    }
    return NULL;
}

/*
 * Adds all the words from the given document to the given index.
 * This function is responsible for deallocating the list and the document name after use.
 */
void index_add_document(index_t *idx, char *document_name, list_t *words, document_t *document) {
    list_iter_t *document_word_iterator = list_createiter(words);
    trie_t *trie = trie_create();
    list_t *words_from_document = list_create(cmp_strs);
    list_addlast(document->document_list, document_name);
    int word_index = 0;

    while (list_hasnext(document_word_iterator)) {
        char *key = process_key(list_next(document_word_iterator));
        if (key != NULL) {
            // accounts for spaces in the file, except for at the beginning
            if (word_index != 0) {
                word_index++;
                list_addlast(words_from_document, " ");
            }
            int *p = malloc(sizeof(int));
            *p = word_index;
            trie_insert(trie, key, p);
            list_addlast(words_from_document, key);
            word_index++;
        }
    }
    map_put(idx->index_map, document_name, trie); // populates trie
    map_put(idx->document_map, document_name, words_from_document); // link all words to a document
}

/*
 * Add indexes from all queries, takes account for multiple words or sentences
 */
list_t *add_result_to_list(list_t *result_list, list_t *current_result) {
    list_iter_t *current_result_list_iter = list_createiter(current_result);
    while (list_hasnext(current_result_list_iter)) {
        list_addlast(result_list, list_next(current_result_list_iter));
    }
    list_destroy(current_result);
    return result_list;
}

/*
 * creates a list of search_hit_t from the results of indexes
 */
list_t *add_search_hit_to_result(list_t *result_set, size_t query_size) {
    list_iter_t *iter = list_createiter(result_set);
    list_t *search_result_list = list_create(cmp_ints);
    while (list_hasnext(iter)) {
        int *elem = list_next(iter);
        search_hit_t *hit = malloc(sizeof(search_hit_t));
        hit->len = query_size;
        hit->location = *(int *) elem;
        list_addfirst(search_result_list, hit);
    }
    list_destroyiter(iter);
    return search_result_list;
}

/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query, document_t *document) {
    // Init variables that apply to all documents
    list_iter_t *document_iterator = list_createiter(document->document_list);
    search_result_t *search_result_object = create_search_result();
    search_result_object->current_document = (char *) get_element_for_head(document->document_list);

    while (list_hasnext(document_iterator)) {
        char query_array[strlen(query)];
        strcpy(query_array, query);
        char *split_query = strtok(query_array, " ");
        char *document_name = list_next(document_iterator);

        // add all words for current document to result:
        map_put(search_result_object->document_map, document_name, map_get(idx->document_map, document_name));

        trie_t *trie = map_get(idx->index_map, document_name);
        list_t *result_set = list_create(cmp_ints);

        while (split_query != NULL) {
            list_t *split_query_result_set = trie_find(trie, split_query);
            if (split_query_result_set != NULL) {
                add_result_to_list(result_set, split_query_result_set);
            }
            split_query = strtok(NULL, " ");
        }

        list_addlast(search_result_object->document_list, document_name);
        if (result_set != NULL) {
            list_t *search_result_list = add_search_hit_to_result(result_set, strlen(query));
            list_sort(search_result_list);
            map_put(search_result_object->search_result_map, document_name, search_result_list);
            list_destroy(result_set);
        }
    }

    list_destroyiter(document_iterator);
    search_result_object->document_iterator = list_createiter(search_result_object->document_list);
    search_result_object->current_result_iterator = list_createiter(
            map_get(search_result_object->search_result_map, search_result_object->current_document));
    return search_result_object;
}

/*
 * Autocomplete searches the given trie for a word containing input.
 * The input string is NULL terminated and contains size letters (excluding null termination).
 * The output string MUST be null terminated.
 */
char *autocomplete(index_t *idx, char *input, size_t size, document_t *document) {
    list_iter_t *it = list_createiter(document->document_list);
    while (list_hasnext(it)) {
        trie_t *trie = map_get(idx->index_map, list_next(it));
        list_t *result_set = trie_find_autcomplete(trie, input, size);
        if (result_set != NULL) {
            list_sort(result_set); // sort autocomplete suggestions in alphabetical order
            list_iter_t *iter = list_createiter(result_set);
            if (list_hasnext(iter)) {
                char *suggested_word = (char *) list_next(iter);
                list_popfirst(result_set);
                return suggested_word;
            }
        }
    }
    return NULL;
}

/* 
 * Return the content of the current document.
 * Subsequent calls to this function should return the next document.
 * This function should only be called once for each document.
 * This function should return NULL if there are no more documents.
 */
char **result_get_content(search_result_t *res) {
    // Only return a result if there are any indexes found for the document
    if (list_hasnext(res->document_iterator) && map_get(res->search_result_map, res->current_document) != NULL) {
        char *current_document = list_next(res->document_iterator);
        list_t *document_words = map_get(res->document_map, current_document);
        list_iter_t *results_for_ui_iterator = list_createiter(document_words);
        int arr_size = list_size(document_words);
        char **arr;
        arr = calloc(list_size(document_words), sizeof(char *));

        while (list_hasnext(results_for_ui_iterator)) {
            for (int i = 0; i < arr_size; ++i) {
                arr[i] = list_next(results_for_ui_iterator);
            }
        }
        list_popfirst(res->document_list);
        list_destroyiter(results_for_ui_iterator);
        res->current_document = current_document;
        res->current_result_iterator = list_createiter(map_get(res->search_result_map, res->current_document));
        return arr;
    }
    return NULL;
}


/*
 * Get the length of the current document.
 * Subsequent calls should return the length of the same document.
 */
int result_get_content_length(search_result_t *res) {
    return list_size(map_get(res->document_map, res->current_document));
}

/*
 * Get the next result_list from the current query.
 * The result_list should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res) {
    if (list_hasnext(res->current_result_iterator)) {
        search_hit_t *hit = list_next(res->current_result_iterator);
        return hit;
    } else {
        return NULL;
    }
}
