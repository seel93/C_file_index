
#include <string.h>

#include "index.h"
#include "map.h"
#include "printing.h"
#include "trie.h"

/*
 * Implement your index here.
 */ 
struct index
{

};


struct search_result
{

};


// Compare to ints
static inline int cmp_ints(void *a, void *b)
{
    return *((int *)a) - *((int *)b);
}


// Compares two strings without case-sensitivity 
static inline int cmp_strs(void *a, void *b)
{
    return strcasecmp((const char *)a, (const char *)b);
}



index_t *index_create()
{
    return NULL;
}



void index_destroy(index_t *index)
{
    
}



void index_add_document(index_t *idx, char *document_name, list_t *words)
{
    
}


search_result_t *index_find(index_t *idx, char *query)
{
    return NULL;
}


char *autocomplete(index_t *idx, char *input, size_t size)
{
    return NULL;
}


char **result_get_content(search_result_t *res)
{
    return NULL;
}


int result_get_content_length(search_result_t *res)
{
    return NULL;
}


search_hit_t *result_next(search_result_t *res)
{
    return NULL;
}
