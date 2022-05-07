
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
    map_t *document_map;
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
    map_t *search_result_map;
    list_t *document_list;
    map_t *document_map;
    list_iter_t *document_iterator;
    list_iter_t *current_result_iterator;
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
    document->document_map = map_create(cmp_strs, djb2);
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
    map_destroy(document->document_map, NULL, NULL);
    list_destroy(document->document_list);
    free(document);
}


/*
 * Destroys the given index.  Subsequently accessing the index will
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
 * Add space to every word readability in the console
 */
char *add_space_to_string(char *str) {
    char *name_with_extension;
    name_with_extension = malloc(strlen(str) + 1 + 4);
    strcpy(name_with_extension, str);
    strcat(name_with_extension, " ");
    return name_with_extension;
}


/*
 * Adds all the words from the given document to the given index.
 * This function is responsible for deallocating the list and the document name after use.
 */
void index_add_document(index_t *idx, char *document_name, list_t *words, document_t *document) {
    list_iter_t *it;
    it = list_createiter(words);
    trie_t *trie = trie_create();
    list_t *words_from_document = list_create(cmp_strs);
    list_addlast(document->document_list, document_name);
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
            list_addlast(words_from_document, add_space_to_string(key));
            word_index++;
        }
    }
    map_put(idx->index_map, document_name, trie);
    map_put(idx->document_map, document_name, words_from_document);
    map_put(document->document_map, document_name, words_from_document);
}

/*
 * Add indexes from all queries with multiple words or sentence
 */
list_t *add_result_to_list(list_t *result_list, list_t *current_result) {
    list_sort(result_list);
    list_sort(current_result);
    if (list_size(result_list) == 0) {
        list_iter_t *current_result_list_iter = list_createiter(current_result);
        while (list_hasnext(current_result_list_iter)) {
            list_addlast(result_list, list_next(current_result_list_iter));
        }
        DEBUG_PRINT("empty list, add all elements \n");
    } else {
        DEBUG_PRINT("check for corresponding indexes and modify list \n");
        list_iter_t *current_result_list_iter = list_createiter(current_result);
        list_iter_t *result_list_iter = list_createiter(result_list);

        while (list_hasnext(current_result_list_iter) && list_hasnext(result_list_iter)) {
            int current_index = *(int *) list_next(result_list_iter);
            int next_index = *(int *) list_next(current_result_list_iter);
            DEBUG_PRINT(" sentence indexes: %d %d \n", current_index, next_index);
        }
    }
    return result_list;
}

/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query, document_t *document) {
    // Init variables that apply to all documents
    list_iter_t *document_iterator = list_createiter(document->document_list);
    search_result_t *search_result_object = create_search_result();


    while (list_hasnext(document_iterator)) {
        char query_array[strlen(query)];
        strcpy(query_array, query);
        char *split_query = strtok(query_array, " ");
        char *document_name = list_next(document_iterator);

        map_put(search_result_object->document_map, document_name, map_get(idx->document_map, document_name));

        trie_t *trie = map_get(idx->index_map, document_name);
        list_t *result_set = list_create(cmp_ints);

        while (split_query != NULL) {
            list_t *split_query_result_set = trie_find(trie, split_query);
            if (split_query_result_set != NULL) {
                DEBUG_PRINT(" %d %s %s\n", list_size(split_query_result_set), document_name, split_query);
                add_result_to_list(result_set, split_query_result_set);
            }
            split_query = strtok(NULL, " ");
        }

        list_addlast(search_result_object->document_list, document_name);
        if (result_set != NULL) {
            DEBUG_PRINT("assinging search_hit for result %s \n", document_name);
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
            search_result_object->current_document = document_name;
        search_result_object->current_result_iterator = list_createiter(search_result_list);
        }
    }
    search_result_object->document_iterator = list_createiter(search_result_object->document_list);
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
    if (list_hasnext(res->document_iterator)) {
        list_t *document_words = map_get(res->document_map, list_next(res->document_iterator));
        list_iter_t *results_for_ui_iterator = list_createiter(document_words);
        int arr_size = list_size(document_words);
        char **arr;
        arr = calloc(list_size(document_words), sizeof(char *));

        while (list_hasnext(results_for_ui_iterator)) {
            for (int i = 0; i < arr_size; ++i) {
                arr[i] = (char *) list_next(results_for_ui_iterator);
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
    return list_size(map_get(res->document_map, res->current_document));
}


/*
 * Get the next result_list from the current query.
 * The result_list should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res) {
    list_t *result_list = map_get(res->search_result_map, res->current_document);
    list_sort(result_list);
    if (list_hasnext(res->current_result_iterator)) {
        search_hit_t *hit = list_next(res->current_result_iterator);
        DEBUG_PRINT("location for query: %d \n", hit->location);
        return hit;
    } else {
        return NULL;
    }
}
