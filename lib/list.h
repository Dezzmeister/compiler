/**
 * Generic linked list implementation. This linked list can be used as a stack, queue, or deque.
 * Functions are generally prefixed with `ll` and possibly also with a type specifier. Generics
 * are implemented using type macros, which need to be defined before including this file. For an
 * explanation of this method, see generics.txt.
 *
 * Nodes are allocated on the heap, so make sure you call `ll_free` when you're done with the
 * linked list.
 *
 * If you only need to pop from one end of the linked list, make sure you're popping from the
 * front. Popping from the front can be done in O(1) time, whereas popping from the back takes
 * O(n). Pushing to either the front of back can be done in constant time.
 */
#ifdef TYPE
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"
#include "types.h"

#ifndef TYPED
// The caller is not adding a prefix to type names, so
// just use the default type name
#define TYPED(NAME) NAME
#endif

typedef bool (*TYPED(CmpFunc))(TYPE, TYPE);

typedef struct TYPED(Node) TYPED(Node);

struct TYPED(Node) {
    struct TYPED(Node) *next;
    TYPE data;
};

/**
 * Linked list with heap-allocated nodes. These struct members should NEVER be directly
 * modified. Use the associated functions or write one if the functionality you want is
 * missing.
 */
typedef struct TYPED(LinkedList) {
    TYPED(Node) *root;
    TYPED(Node) *last;
    long length;
} TYPED(LinkedList);

DECL_OPTION(TYPED(LinkedList))

/**
 * Create a new linked list with zeroed members. It's important that the root and last
 * nodes are NULL and the length is 0 for a new linked list.
 */
TYPED(LinkedList) TYPED(_ll_new) ();

/**
 * Adds an item to the end (back) of the linked list in O(1) time.
 */
StatusCode TYPED(_ll_pushb) (TYPED(LinkedList) *list, TYPE item);

/**
 * Removes the last item in the linked list and returns it or an empty option if the list is
 * empty.
 */
OPT(TYPE) TYPED(_ll_popb) (TYPED(LinkedList) *list);

/**
 * Adds an item to the front of the linked list in O(1) time.
 */
StatusCode TYPED(_ll_pushf) (TYPED(LinkedList) *list, TYPE item);

/**
 * Removes an item from the front of the linked list in O(1) time and returns it or an empty option
 * if the list is empty.
 */
OPT(TYPE) TYPED(_ll_popf) (TYPED(LinkedList) *list);

/**
 * Searches the list and returns true if the item exists in the list. Best case O(1), worst
 * case O(n).
 */
bool TYPED(_ll_includes) (TYPED(LinkedList) *list, TYPED(CmpFunc) cmp, TYPE item);

void TYPED(_ll_remove) (TYPED(LinkedList) *list, TYPED(Node) *node, TYPED(Node) *prev);

/**
 * Frees any remaining nodes held by the linked list in O(n) time. At this point the linked list
 * is invalid and should be freed or dropped from scope (if allocated on the stack). Since the caller
 * created the linked list, it is the caller's responsibility to do this.
 */
void TYPED(_ll_free) (TYPED(LinkedList) *list);


// IMPLEMENTATION


TYPED(LinkedList) TYPED(_ll_new) () {
    TYPED(LinkedList) out = {
        .root = NULL,
        .last = NULL,
        .length = 0
    };

    return out;
}

StatusCode TYPED(_ll_pushb) (TYPED(LinkedList) *list, TYPE item) {
    TYPED(Node) *node = malloc(sizeof *node);
    
    if (node == NULL) {
        return ERROR_OUT_OF_MEM;
    }
    
    node->next = NULL;
    node->data = item;

    if (list->root == NULL) {
        list->root = node;
        list->last = node;
        list->length = 1;

        return STATUS_OK;
    }

    (list->last)->next = node;
    list->last = node;
    list->length++;

    return STATUS_OK;
}

OPT(TYPE) TYPED(_ll_popb) (TYPED(LinkedList) *list) {
    if (list->root == NULL) {
        return OPT_NONE(TYPE);
    }

    if (list->root == list->last) {
        TYPE item = list->root->data;
        free(list->root);
        list->root = NULL;
        list->last = NULL;
        list->length = 0;
        
        return OPT_SOME(TYPE, item);
    }

    TYPED(Node) *curr = list->root;
    
    // Loop through nodes until we reach the one before the last
    while (curr->next != list->last) {
        curr = curr->next;
    }

    TYPE item = list->last->data;

    free(list->last);
    curr->next = NULL;

    list->last = curr;
    list->length--;

    return OPT_SOME(TYPE, item);
}

StatusCode TYPED(_ll_pushf) (TYPED(LinkedList) *list, TYPE item) {
    TYPED(Node) *node = malloc(sizeof *node);
    
    if (node == NULL) {
        return ERROR_OUT_OF_MEM;
    }

    node->next = list->root;
    node->data = item;

    if (list->root == NULL) {
        list->root = node;
        list->last = node;
        list->length = 1;
        
        return STATUS_OK;
    }

    list->root = node;
    list->length++;

    return STATUS_OK;
}

OPT(TYPE) TYPED(_ll_popf) (TYPED(LinkedList) *list) {
    if (list->root == NULL) {
        return OPT_NONE(TYPE);
    }

    if (list->root == list->last) {
        TYPE item = list->root->data;
        free(list->root);
        list->root = NULL;
        list->last = NULL;
        list->length = 0;
        
        return OPT_SOME(TYPE, item);
    }

    TYPE item = list->root->data;
    TYPED(Node) *next = list->root->next;

    free(list->root);
    list->root = next;
    list->length--;

    return OPT_SOME(TYPE, item);
}

bool TYPED(_ll_includes) (TYPED(LinkedList) *list, TYPED(CmpFunc) cmp, TYPE item) {
    TYPED(Node) *curr = list->root;

    while (curr != NULL) {
        if (cmp(curr->data, item)) {
            return true;
        }
        curr = curr->next;
    }

    return false;
}

void TYPED(_ll_remove) (TYPED(LinkedList) *list, TYPED(Node) *node, TYPED(Node) *prev) {
    if (list->root == NULL) {
        return;
    }

    // Caller wants to remove the root node, call popf
    if (node == list->root) {
        TYPED(_ll_popf)(list);
        return;
    }

    // Caller wants to remove the last node, call popb
    if (node == list->last) {
        TYPED(_ll_popb)(list);
        return;
    }

    // Caller has given us a node to remove and the one before it.
    // Make sure that prev really does come before node, then remove node
    if (prev != NULL && prev->next == node) {
        prev->next = node->next;
        free(node);
        list->length--;
        return;
    }

    // Caller has given us a node to remove but `prev` is non null and does not
    // actually come before node. The caller messed up, do nothing
    if (prev != NULL) {
        return;
    }
}

void TYPED(_ll_free) (TYPED(LinkedList) *list) {
    if (list->root == NULL) {
        return;
    }

    TYPED(Node) *next = NULL;
    TYPED(Node) *curr = list->root;

    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
}

#endif
