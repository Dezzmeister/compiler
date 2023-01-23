#include <assert.h>
#include <stdlib.h>

#include "../lib/structures.h"
#include "../lib/utils.h"

typedef struct {
    int x;
    int y;
} TestStruct;

DECL_OPTION(TestStruct);
#define TYPE TestStruct
#define TYPED(NAME) TS ## NAME
#include "../lib/vec.h"
#undef TYPED
#undef TYPE

void primitive_logic_test() {
    opt_IntVec vec_opt = Int_vec_new_cap(100);

    assert(vec_opt.present);

    IntVec vec = vec_opt.item;
    assert(!vec.length);
    
    // Capacity will grow by 1.5x each time, as follows:
    // 100 -> 150
    // 150 -> 225
    // 225 -> 337
    // 337 -> 505
    // 505 -> 757
    // 757 -> 1135
    // The capacity should be 1135 by the end of the loop.
    for (int i = 0; i < 1000; i++) {
        Int_vec_push(&vec, i);
    }

    assert(vec.length == 1000);
    assert(vec.capacity == 1135);

    for (int i = 0; i < 1000; i++) {
        assert(vec.v[i] == i);
    }

    Int_vec_shrink(&vec);

    assert(vec.capacity == 1000);

    for (int i = 999; i >= 0; i--) {
        opt_int item_opt = Int_vec_pop(&vec);
        
        assert(item_opt.present);
        assert(item_opt.item == i);
        assert(vec.length == i);
    }

    for (int i = 0; i < 10; i++) {
        opt_int item_opt = Int_vec_pop(&vec);
        
        assert(!item_opt.present);
        assert(!vec.length);
    }

    Int_vec_free(&vec);
}

void struct_logic_test() {
    opt_TSVec vec_opt = TS_vec_new_cap(1000);
    
    assert(vec_opt.present);
    
    TSVec vec = vec_opt.item;
    assert(!vec.length);

    for (int i = 0; i < 1000; i++) {
        TestStruct item = {
            .x = i,
            .y = 2 * i
        };

        assert(!TS_vec_push(&vec, item));
    }

    assert(vec.capacity == 1000);
    assert(vec.length == 1000);

    TS_vec_shrink(&vec);

    assert(vec.capacity == 1000);
    assert(vec.length == 1000);

    for (int i = 0; i < 1000; i++) {
        assert(vec.v[i].x == i);
        assert(vec.v[i].y == (2 * i));
    }

    TS_vec_free(&vec);
}

int main() {
    primitive_logic_test();
    struct_logic_test();

    return EXIT_SUCCESS;
}
