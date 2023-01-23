/**
 * A generic hashmap with buckets stored on the heap. Buckets are linked lists that store structs containing key/value pairs.
 * Keys can be any type, and so can values. The only requirement is that the caller provides two functions:
 *      
 *      1. A hash function to compute the hash of any given key. Ideally, hashes should be uniformly distributed
 *         and pseudo-random, but also deterministic. Cryptographic hash functions are like this but please don't use
 *         something so heavyweight for a hashmap.
 *      2. A comparison function to determine if two keys are equal.
 *
 * The caller provides these functions when constructing the hashmap, and the hashmap handles the rest
 * after that. There are several associated functions that can be used with the hashmap.
 *
 * Specializing HashMap for Custom Types
 *
 * This works a little differently than some of the other generic data structures like LinkedList and Vec. The trouble
 * here is that HashMap needs to specialize a LinkedList for its internal HMapEntry struct. This means that TYPE and TYPED
 * cannot be used for the HashMap's type arguments, because they need to be used to create a LinkedList. Furthermore, a HashMap
 * has two type arguments, not one, so a single `TYPE` won't do. To create a HashMap for a pair of key/value types, you need to first
 * `DECL_OPTION` your type as usual. Then, you can set either `KEY` and `VAL` or `TYPE1` and `TYPE2` to the key and value types
 * respectively. Finally, set the `TYPED2(NAME)` macro to return a prefixed name for your custom HashMap. The procedure looks
 * something like this:
 *
 *      #include "utils.h"
 *
 *      DECL_OPTION(MyStruct)
 *      #define KEY int
 *      #define VAL MyStruct
 *      #define TYPED2(NAME) IToMS ## NAME
 *      #include "hashmap.h"
 *      
 * The new HashMap type will be called `IToMSHashMap` and will have associated functions like `IToMS_hmap_new`, `IToMS_hmap_put`, etc.
 * See the tests for example HashMap usage.
 */
#if (defined TYPE1 || defined KEY) && (defined TYPE2 || defined VAL)

#ifndef TYPED2
#define TYPED2(NAME) NAME
#endif

#ifdef TYPE1
#define KEY TYPE1
#endif

#ifdef TYPE2
#define VAL TYPE2
#endif

#ifndef _LIB_HASHMAP_H_
#define _LIB_HASHMAP_H_

#define INITIAL_HASHMAP_CAPACITY    100
#define HASHMAP_GROWTH_FACTOR       2.0

#endif

#include "utils.h"
#include "errors.h"
#include "types.h"

typedef struct {
    KEY key;
    VAL val;
} TYPED2(HMapEntry);

// Create a LinkedList type for HMapEntry
DECL_OPTION(TYPED2(HMapEntry))

#undef TYPE
#undef TYPED
#define TYPE TYPED2(HMapEntry)
#define TYPED(NAME) M_CONC(TYPED2(HMapEntry), M_CONC(_, NAME))
#include "list.h"

typedef int (*TYPED2(HashFunc))(KEY);

typedef bool (*TYPED2(KeyCmp))(KEY, KEY);

typedef struct {
    TYPED(LinkedList) *buckets;
    long size;
    long capacity;
    TYPED2(HashFunc) hash_func;
    TYPED2(KeyCmp) key_cmp;
} TYPED2(HashMap);

DECL_OPTION(TYPED2(HashMap))

/**
 * Creates a HashMap with the given hash function and key comparison function. By default, the HashMap allocates
 * 100 buckets on the heap. The number of buckets can grow as needed, but it will never shrink.
 */
OPT(TYPED2(HashMap)) TYPED2(_hmap_new) (TYPED2(HashFunc) hash_func, TYPED2(KeyCmp) key_cmp);

/**
 * Creates a HashMap with the given hash function, key comparison function, and initial number of buckets.
 * The number of buckets can grow as needed, but it will never shrink.
 */
OPT(TYPED2(HashMap)) TYPED2(_hmap_new_cap) (TYPED2(HashFunc) hash_func, TYPED2(KeyCmp) key_cmp, long capacity);

/**
 * Puts a key/value pair in the hashmap. If there is already an entry with the given key, then the value for that key
 * is set to this one.
 */
StatusCode TYPED2(_hmap_put) (TYPED2(HashMap) *map, KEY key, VAL val);

/**
 * Gets the value mapped to by a given key, or returns an empty option if the key is not in the hashmap.
 */
OPT(VAL) TYPED2(_hmap_get) (TYPED2(HashMap) *map, KEY key);

/**
 * Removes the entry with the given key. If there was such an entry in the hashmap, returns the associated value.
 * Otherwise returns an empty option.
 */
OPT(VAL) TYPED2(_hmap_remove) (TYPED2(HashMap) *map, KEY key);

/**
 * Frees all memory held by the hashmap. This MUST be called when you're done using the hashmap, even if the hashmap
 * is allocated on the stack. The hashmap still allocates some data on the heap and that always needs to be cleaned up.
 */
void TYPED2(_hmap_free) (TYPED2(HashMap) *map);


// IMPLEMENTATION


static StatusCode TYPED2(_resize) (TYPED2(HashMap) *map) {
    long new_capacity = HASHMAP_GROWTH_FACTOR * map->capacity;
    // This is a malloc because we need to move every entry in the hashmap anyway,
    // so there's no point in keeping the same memory. That would just make it harder
    // to move entries around.
    TYPED(LinkedList) *new_buckets = malloc(sizeof *new_buckets * new_capacity);

    if (new_buckets == NULL) {
        return ERROR_OUT_OF_MEM;
    }

    // Loop through the new buckets and create linked lists for them
    for (int i = 0; i < new_capacity; i++) {
        new_buckets[i] = TYPED(_ll_new)();
    }

    // Move every entry in the map into the new buckets
    for (int i = 0; i < map->capacity; i++) {
        OPT(TYPED2(HMapEntry)) entry_opt;

        while ((entry_opt = TYPED(_ll_popf)(&map->buckets[i])).present) {
            TYPED2(HMapEntry) entry = entry_opt.item;
            const int new_pos = map->hash_func(entry.key) % new_capacity;
            TYPED(_ll_pushf)(&new_buckets[new_pos], entry);
        }

        TYPED(_ll_free)(&map->buckets[i]);
    }
    
    free(map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_capacity;

    return STATUS_OK;
}

OPT(TYPED2(HashMap)) TYPED2(_hmap_new) (TYPED2(HashFunc) hash_func, TYPED2(KeyCmp) key_cmp) {
    return TYPED2(_hmap_new_cap)(hash_func, key_cmp, INITIAL_HASHMAP_CAPACITY);
}

OPT(TYPED2(HashMap)) TYPED2(_hmap_new_cap) (TYPED2(HashFunc) hash_func, TYPED2(KeyCmp) key_cmp, long capacity) {
    TYPED(LinkedList) *buckets = malloc(sizeof *buckets * capacity);

    if (buckets == NULL) {
        return OPT_NONE_ERR(TYPED2(HashMap), ERROR_OUT_OF_MEM);
    }

    for (int i = 0; i < capacity; i++) {
        buckets[i] = TYPED(_ll_new)();
    }

    TYPED2(HashMap) out = {
        .buckets = buckets,
        .capacity = capacity,
        .size = 0,
        .hash_func = hash_func,
        .key_cmp = key_cmp
    };

    return OPT_SOME(TYPED2(HashMap), out);
}

StatusCode TYPED2(_hmap_put) (TYPED2(HashMap) *map, KEY key, VAL val) {
    int hash = map->hash_func(key);
    int pos = hash % map->capacity;

    TYPED(LinkedList) *list = map->buckets + pos;
    // Handle best case (and most likely) scenario first - the desired bucket is empty
    if (!list->length) {
        // We can insert the value here and increase the size of the map
        TYPED2(HMapEntry) entry = {
            .key = key,
            .val = val
        };
        StatusCode res = TYPED(_ll_pushb)(list, entry);

        if (res) {
            return res;
        }
        
        map->size++;

        if (map->size == (map->capacity + 1)) {
            const StatusCode error = TYPED2(_resize)(map);

            if (error) {
                return error;
            }
        }

        return STATUS_OK;
    }

    TYPED(Node) *curr = list->root;

    while (curr != NULL) {
        if (map->key_cmp(curr->data.key, key)) {
            curr->data.val = val;
            return STATUS_OK;
        }
        curr = curr->next;
    }

    TYPED2(HMapEntry) entry = {
        .key = key,
        .val = val
    };
    StatusCode res = TYPED(_ll_pushb)(list, entry);
    
    if (res) {
        return res;
    }

    map->size++;

    if (map->size == (map->capacity + 1)) {
        return TYPED2(_resize)(map);
    }

    return STATUS_OK;
}

OPT(VAL) TYPED2(_hmap_get) (TYPED2(HashMap) *map, KEY key) {
    const int pos = map->hash_func(key) % map->capacity;
    
    TYPED(Node) *curr = map->buckets[pos].root;

    while (curr != NULL && !map->key_cmp(curr->data.key, key)) {
        curr = curr->next;
    }

    if (curr != NULL) {
        return OPT_SOME(VAL, curr->data.val);
    }

    return OPT_NONE(VAL);
}

OPT(VAL) TYPED2(_hmap_remove) (TYPED2(HashMap) *map, KEY key) {
    const int pos = map->hash_func(key) % map->capacity;

    TYPED(Node) *prev = NULL;
    TYPED(Node) *curr = map->buckets[pos].root;

    while (curr != NULL && !map->key_cmp(curr->data.key, key)) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        return OPT_NONE(VAL);
    }

    VAL val = curr->data.val;

    TYPED(_ll_remove)(&map->buckets[pos], curr, prev);
    map->size--;

    return OPT_SOME(VAL, val);
}

void TYPED2(_hmap_free) (TYPED2(HashMap) *map) {
    for (long i = 0; i < map->capacity; i++) {
        TYPED(_ll_free)(&map->buckets[i]);
    }

    free(map->buckets);
}

#undef TYPED
#undef TYPE
#undef VAL
#undef KEY
#undef TYPE1
#undef TYPE2
#endif
