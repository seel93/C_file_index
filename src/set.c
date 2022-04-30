//
// Created by oystein on 30.04.2022.
//

#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "set.h"
#include "printing.h"


struct set {
    list_t *list;
    cmpfunc_t cmpfunc;
    int is_sorted;
};

/*
 * Creates a new set using the given comparison function
 * to compare elements of the set.
 */
set_t *set_create(cmpfunc_t cmpfunc) {
    set_t *set = malloc(sizeof(set_t));
    set->list = list_create(cmpfunc);
    set->cmpfunc = cmpfunc;
    set->is_sorted = 0;

    if (set->list == NULL)
        return NULL;

    return set;
}

/*
 * Destroys the given set.  Subsequently accessing the set
 * will lead to undefined behavior.
 */
void set_destroy(set_t *set) {
    list_destroy(set->list);
    free(set);
}

void set_sort(set_t *set) {
    printf("sorting");
    list_sort(set->list);
    set->is_sorted = 1;
}

/*
 * Returns the size (cardinality) of the given set.
 */
int set_size(set_t *set) {
    return list_size(set->list);
}

/*
 * Adds the given element to the given set.
 */
void set_add(set_t *set, void *elem) {
    if (set_contains(set, elem) == 1) {

        return;
    }
    list_addlast(set->list, elem);
}

/*
 * Returns 1 if the given element is contained in
 * the given set, 0 otherwise.
 */
int set_contains(set_t *set, void *elem) {
    if (list_contains(set->list, elem) == 1) {
        DEBUG_PRINT("set already has this element");
        return 1;
    }
    return 0;
}

/*
 * Returns the union of the two given sets; the returned
 * set contains all elements that are contained in either
 * a or b.
 */
set_t *set_union(set_t *a, set_t *b) {
    set_t *union_set = set_create(a->cmpfunc);
    set_iter_t *iter_a = set_create_iter(a);

    while (set_has_next(iter_a)) {
        void *item_a = set_next(iter_a);

        set_add(union_set, item_a);
    }

    set_iter_t *iter_b = set_create_iter(b);

    while (set_has_next(iter_b)) {
        void *item_b = set_next(iter_b);

        set_add(union_set, item_b);

    }
    set_destroy_iter(iter_a);
    set_destroy_iter(iter_b);

    return union_set;
}

/*
 * Returns the intersection of the two given sets; the
 * returned set contains all elements that are contained
 * in both a and b.
 */
set_t *set_intersection(set_t *a, set_t *b) {
    set_t *intersection_set = set_create(a->cmpfunc);
    set_iter_t *iter_a = set_create_iter(a);
    void *item_a = set_next(iter_a);

    while (item_a != NULL) {
        if (set_contains(b, item_a) == 1)
            set_add(intersection_set, item_a);
        item_a = set_next(iter_a);
    }
    set_destroy_iter(iter_a);

    return intersection_set;
}

/*
 * Returns the set difference of the two given sets; the
 * returned set contains all elements that are contained
 * in a and not in b.
 */
set_t *set_difference(set_t *a, set_t *b) {
    set_t *difference_set = set_create(a->cmpfunc);
    set_iter_t *iter_a = set_create_iter(a);
    void *item_a = set_next(iter_a);

    while (item_a != NULL) {

        if (set_contains(b, item_a) == 0)
            set_add(difference_set, item_a);

        item_a = set_next(iter_a);

    }
    set_destroy_iter(iter_a);
    return difference_set;
}

/*
 * Returns a copy of the given set.
 */
set_t *set_copy(set_t *set) {
    set_t *copied_set = set_create(set->cmpfunc);

    copied_set->is_sorted = set->is_sorted;

    set_t *iter = set_create_iter(set);

    void *item = set_next(iter);

    while (item != NULL) {
        set_add(copied_set, item);

        item = set_next(iter);
    }

    return copied_set;
}


struct set_iter {
    list_iter_t *iter;
};


set_iter_t *set_create_iter(set_t *set) {
    set_sort(set);
    set_iter_t *iter_set = malloc(sizeof(set_iter_t));

    if (iter_set == NULL)
        return NULL;

    list_iter_t *iter_list = list_createiter(set->list);

    if (iter_list == NULL) {
        free(iter_set);
        return NULL;
    }

    iter_set->iter = iter_list;
    return iter_set;
}

void set_destroy_iter(set_iter_t *iter) {
    list_destroyiter(iter->iter);
    free(iter);
}

int set_has_next(set_iter_t *iter) {
    return list_hasnext(iter->iter);
}

void *set_next(set_iter_t *iter) {
    return list_next(iter->iter);
}

