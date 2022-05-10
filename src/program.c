#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "index.h"
#include "printing.h"

index_t *idx = NULL;
document_t *document = NULL;

int mystrcmp(void *a, void *b) {
    return strcmp((const char *) a, (const char *) b);
}


void initialize_index(char *root_dir) {
    char *relpath, *fullpath;
    list_t *files, *words;
    list_iter_t *iter;


    if (is_valid_directory(root_dir)) {
        files = find_files(root_dir);
    } else if (is_valid_file(root_dir)) {
        files = list_create(mystrcmp);

        list_addfirst(files, root_dir);
    } else {
        ERROR_PRINT("Root dir %s is not a valid directory or file\n", root_dir);
        exit(1);
    }


    idx = index_create();
    document = document_create();
    if (idx == NULL) {
        ERROR_PRINT("Failed to create index\n");
    }

    iter = list_createiter(files);

    while (list_hasnext(iter)) {
        relpath = (char *) list_next(iter);
        fullpath = concatenate_strings(2, root_dir, relpath);

        words = list_create(mystrcmp);
        tokenize_file(fullpath, words);
        index_add_document(idx, relpath, words, document);

        free(fullpath);

        list_destroy(words);
    }

    list_destroyiter(iter);
    list_destroy(files);
}

int main(int argc, char **argv) {

    // Initial program
    if (argc != 2) {
        ERROR_PRINT("Usage: %s <root-dir>\n", argv[0]);
        return 1;
    }

    char *root_dir = argv[1];
    // init index two .txt files, add test.txt from /tmp for additional file
    unsigned long long init_index_creation_time = gettime();
    initialize_index(root_dir);
    unsigned long long init_index_cration_diff_time = gettime() - init_index_creation_time;
    INFO_PRINT("Build index with n files:  %llu \n", init_index_cration_diff_time);


    // init single two .txt files: to run this test move files from /data
    //INFO_PRINT("Init index single file \n");
    //unsigned long long init_index_single_file_creation_time = gettime();
    //initialize_index(root_dir);
    //unsigned long long init_index_single_file_creation_diff_time = gettime() - init_index_single_file_creation_time;
    //INFO_PRINT("%llu \n", init_index_single_file_creation_diff_time);

    // ------------------------------------- Testing index ------------------------------------------------------ //

    // testing single word:
    char *a = "relief";
    unsigned long long index_single_word_search_time = gettime();
    search_result_t *res = index_find(idx, a, document);
    result_get_content(res);
    unsigned long long index_single_word_search_diff_time = gettime() - index_single_word_search_time;
    INFO_PRINT("Index single word search time %llu \n", index_single_word_search_diff_time);

    // testing mutiple words:
    char *b = "hamlet treasure bernardo the there of";
    unsigned long long index_multilpe_words_search_time = gettime();
    search_result_t *res_multiple_words = index_find(idx, b, document);
    unsigned long long index_multilpe_words_search_diff_time = gettime() - index_multilpe_words_search_time;
    INFO_PRINT("Index multiple words(6) search time %llu \n", index_multilpe_words_search_diff_time);

    // testing autocomplete:
    char *d = "haml";
    unsigned long long index_single_word_autocomplete_time = gettime();
    char *res_autocomplete = autocomplete(idx, d, strlen(d), document);
    unsigned long long index_single_word_autocomplete_diff_time = gettime() - index_single_word_autocomplete_time;
    INFO_PRINT("Index autocomplete time %llu \n", index_single_word_autocomplete_diff_time);

    // ----------------------------------------- Testing trie --------------------------------------------- //

    // init trie:
    trie_t  *trie = trie_create();
    trie_insert(trie, "they", (void *) 0);
    trie_insert(trie, "there", (void *) 1);
    trie_insert(trie, "themselves", (void *) 2);
    trie_insert(trie, "theyre", (void *) 3);
    trie_insert(trie, "their", (void *) 4);
    trie_insert(trie, "them", (void *) 4);

    unsigned long long trie_search_time = gettime();
    list_t *result = trie_find(trie, "themselves");
    unsigned long long trie_search_diff_time = gettime() - trie_search_time;
    INFO_PRINT("trie search size of result: %d \n", list_size(result));
    INFO_PRINT("Trie search time %llu \n", trie_search_diff_time);

    unsigned long long trie_search_auto_time = gettime();
    list_t *result2 = trie_find_autcomplete(trie, "the", 3);
    unsigned long long trie_search_auto_diff_time = gettime() - trie_search_auto_time;
    INFO_PRINT("number of suggested words %d \n", list_size(result2));
    INFO_PRINT("trie auto time %llu \n", trie_search_auto_diff_time);

    return 0;
}

