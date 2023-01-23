#include <assert.h>
#include <stdlib.h>

/**
 * structures.h contains several data structures for primitive types. Include this instead of specializing
 * them yourself to avoid conflicting types.
 */
#include "../lib/structures.h"

/**
 * Several macros including the DECL_OPTION macro which is used a few lines down
 */
#include "../lib/utils.h"

/**
 * A custom struct that we want to store in a LinkedList. NOTE that we don't want to store
 * pointers to the struct, we want to store wholesale structs in the linked list. This is not one
 * of the primitive types specialized in structures.h, so we will need to specialize LinkedList
 * for this type ourselves.
 *
 * Ideally, the specializations should be done in one place to avoid specializing a data structure
 * for the same type twice and creating duplicate type definitions.
 */
typedef struct {
    int x;
    int y;
} TestStruct;

/**
 * Specialization of LinkedList for our custom struct, TestStruct. See structures.h for a detailed explanation.
 * Basically, we first need to generate an `opt_` wrapper for our custom type with DECL_OPTION. Then we need
 * to define the typename (TestStruct), and the type prefix (TS) for our specialized LinkedList.
 * Then we include list.h and undefine TYPED and TYPE. At this point, the type TSLinkedList exists along with
 * associated functions TS_ll_new, TS_ll_free, TS_ll_pushf, etc. A TSLinkedList is a linked list containing
 * objects of type TestStruct (not pointers to TestStruct!).
 */
DECL_OPTION(TestStruct)
#define TYPE TestStruct
#define TYPED(NAME) TS ## NAME
#include "../lib/list.h"
#undef TYPED
#undef TYPE

bool int_cmp(int a, int b) {
    return a == b;
}

void primitive_logic_test() {
    IntLinkedList list = Int_ll_new();

    Int_ll_pushf(&list, 3);
    Int_ll_pushf(&list, 2);
    Int_ll_pushf(&list, 1);

    Int_ll_pushb(&list, 4);
    Int_ll_pushb(&list, 5);
    Int_ll_pushb(&list, 6);

    assert(list.length == 6);
    assert(Int_ll_includes(&list, &int_cmp, 2));
    assert(!Int_ll_includes(&list, &int_cmp, 7));

    for (int i = 1; i <= 6; i++) {
        opt_int item_opt = Int_ll_popf(&list);

        assert(item_opt.present);
        assert(item_opt.item == i);
        assert(list.length == (6 - i));
    }

    for (int i = 0; i < 10; i++) {
        Int_ll_popf(&list);
        Int_ll_popb(&list);
        assert(list.length == 0);
    }

    for (int i = 0; i < 100; i++) {
        Int_ll_pushb(&list, i);
    }

    assert(list.length == 100);

    // Using the less efficient pop
    for (int i = 99; i >= 0; i--) {
        opt_int item_opt = Int_ll_popb(&list);

        assert(item_opt.present);
        assert(!item_opt.error);
        assert(item_opt.item == i);
    }
    
    Int_ll_free(&list);
}

void struct_queue_test() {
    TSLinkedList list = TS_ll_new();

    for (int i = 0; i < 100; i++) {
        TestStruct item = {
            .x = i,
            .y = 2 * i
        };

        TS_ll_pushb(&list, item);
        assert(list.length == (i + 1));
    }

    for (int i = 0; i < 100; i++) {
        opt_TestStruct item_opt = TS_ll_popf(&list);

        assert(item_opt.present);
        TestStruct item = item_opt.item;

        assert(item.x == i);
        assert(item.y == (2 * i));
    }

    TS_ll_free(&list);
}

int main() {
    primitive_logic_test();
    struct_queue_test();

    return EXIT_SUCCESS;
}
