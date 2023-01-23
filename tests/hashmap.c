#include <assert.h>
#include <stdlib.h>
#include "utils.h"

#define KEY int
#define VAL double
#define TYPED2(NAME) IToD ## NAME
#include "../lib/hashmap.h"
#undef TYPED2
#undef VAL
#undef KEY

typedef struct {
    int x;
    int y;
} TestStruct;

DECL_OPTION(TestStruct)
#define KEY int
#define VAL TestStruct
#define TYPED2(NAME) IToTS ## NAME
#include "../lib/hashmap.h"
#undef TYPED2
#undef VAL
#undef KEY

int itod_hash_func(int key) {
    return key;
}

bool int_cmp(int a, int b) {
    return a == b;
}

void primitive_test() {
    opt_IToDHashMap map_opt = IToD_hmap_new(&itod_hash_func, &int_cmp);
    assert(map_opt.present);

    IToDHashMap map = map_opt.item;

    // Insert a bunch of entries
    for (int i = 0; i < 200; i++) {
        StatusCode error = IToD_hmap_put(&map, i, i * 2.0);
        assert(!error);
        assert(map.size == (i + 1));
    }

    // Get each val without removing it
    for (int i = 0; i < 200; i++) {
        opt_double val_opt = IToD_hmap_get(&map, i);
        assert(val_opt.present);

        double val = val_opt.item;
        assert(val == (i * 2.0));
    }

    for (int i = 199; i >= 0; i--) {
        opt_double val_opt = IToD_hmap_remove(&map, i);
        assert(val_opt.present);

        double val = val_opt.item;
        assert(val == (i * 2.0));

        assert(map.size == i);
    }

    assert(map.size == 0);

    IToD_hmap_free(&map);
}

void struct_test() {
    opt_IToTSHashMap map_opt = IToTS_hmap_new(&itod_hash_func, &int_cmp);
    assert(map_opt.present);

    IToTSHashMap map = map_opt.item;

    for (int i = 0; i < 200; i++) {
        TestStruct item = {
            .x = i,
            .y = 2 * i
        };
        StatusCode error = IToTS_hmap_put(&map, i, item);
        assert(!error);
        assert(map.size == (i + 1));
    }

    for (int i = 0; i < 200; i++) {
        opt_TestStruct val_opt = IToTS_hmap_get(&map, i);
        assert(val_opt.present);

        TestStruct val = val_opt.item;
        assert(val.x == i);
        assert(val.y == (2 * i));
    }

    for (int i = 0; i < 200; i++) {
        opt_TestStruct val_opt = IToTS_hmap_remove(&map, i);
        assert(val_opt.present);

        TestStruct val = val_opt.item;
        assert(val.x == i);
        assert(val.y == (2 * i));

        assert(map.size == (199 - i));
    }

    for (int i = 0; i < 10; i++) {
        IToTS_hmap_remove(&map, 5);
        assert(map.size == 0);
    }

    IToTS_hmap_free(&map);
}

int main() {
    primitive_test();
    struct_test();

    return EXIT_SUCCESS;
}
