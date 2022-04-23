#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "list.h"
#include "index.h"
#include "printing.h"
#include "ui.h"


index_t *idx = NULL;

static void printwords(char *prefix, list_t *words) {
    list_iter_t *it;

    it = list_createiter(words);
    printf("%s: ", prefix);
    while (list_hasnext(it)) {
        printf("%s", list_next(it));
    }
    printf("\n");
    list_destroyiter(it);
}


int mystrcmp(void *a, void *b) {
    return strcmp((const char *) a, (const char *) b);
}

static void interrupt_signal(int signo) {
    index_destroy(idx);
    ui_deinit();
    INFO_PRINT("Interrupt signal caught (%d) - Exiting...\n", signo);
    exit(0);
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
    if (idx == NULL) {
        ERROR_PRINT("Failed to create index\n");
    }

    iter = list_createiter(files);

    while (list_hasnext(iter)) {
        relpath = (char *) list_next(iter);
        fullpath = concatenate_strings(2, root_dir, relpath);

        words = list_create(mystrcmp);
        tokenize_file(fullpath, words);
        index_add_document(idx, relpath, words);

        free(fullpath);

        list_destroy(words);
    }

    list_destroyiter(iter);
    list_destroy(files);
}

void main_program_loop() {
    if (signal(SIGINT, interrupt_signal) == SIG_ERR) {
        ERROR_PRINT("An error occurred while setting a signal handler.\n");
        exit(1);
    }

    while (1) {
        char *query = ui_main(idx);
        search_result_t *res = index_find(idx, query);
        ui_result(res);
    }
}


int main(int argc, char **argv) {

    // Initial program
    if (argc != 2) {
        ERROR_PRINT("Usage: %s <root-dir>\n", argv[0]);
        return 1;
    }

    /*
    char *root_dir = argv[1];
    initialize_index(root_dir);
    char *a = "of";
    index_find(idx, a);



    ui_init();
    main_program_loop();
     */

    // ADT operations testing ----------------------------------------
    char *a = "foo";
    char *b = "bar";
    char *c = "get";
    char *d = "rect";
    char *e = "im";
    char *f = "fucked";
    char *g = "foot";
    char *h = "ball";
    char *i = "footy";
    char *j = "table";
    char *k = "more";
    char *l = "major";

    //list_t *list = list_create(mystrcmp);
    //list_addlast(list, 'hei');
    //list_addlast(list, 'hade');
    //list_addlast(list, 'sneiks');
    //index_t *index = index_create();
    //index_add_document(index,'d', list);




    /*
    */
    trie_t *trie = trie_create();
    trie_insert(trie, a, b);
    //trie_insert(trie, c, d);
    //trie_insert(trie, e, f);
    //trie_insert(trie, g, h);
    trie_insert(trie, i, j);
    trie_insert(trie, i, k);
    trie_insert(trie, i, l);


    //char *res1 = trie_find(trie, "foo");
    //char *res2 = trie_find(trie, "get");
    //char *res3 = trie_find(trie, "im");
    //char *res4 = trie_find(trie, "foot");
    char *res5 = trie_find(trie, "footy");


    //DEBUG_PRINT("%s \n", res1);
    //DEBUG_PRINT("%s \n", res2);
    //DEBUG_PRINT("%s \n", res3);
    //DEBUG_PRINT("%s \n", res4);
    DEBUG_PRINT("%s \n", res5);
//    map_t *map = map_create(djb2, compare_strings);
//    int n = map_haskey(map, a);
//    int m = map_haskey(map, c);
//
//    printf("%p", n);
//    printf("%p", m);
//
//    map_put(map, a, b);
//    map_put(map, c, d);
//    void *elem = map_get(map, a);
//
//    printf("%p", &elem);

    return 0;
}