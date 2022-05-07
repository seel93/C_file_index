#ifndef UI_H
#define UI_H

#include "index.h"

#define MIN_SUGGESTION_LEN 3

void ui_init();

void ui_deinit();

char *ui_main(index_t *idx, document_t *document);

void ui_result(search_result_t *res);

#endif