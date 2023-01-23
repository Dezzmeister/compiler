/**
 * Heap-allocated, generic growable array implementation. Similar to ArrayList in Java,
 * std::vector in C++, and Vec in Rust. See generics.txt for an explanation of the way
 * generics are implemented here and how to use these data structures. An example:
 *      
 *      #define TYPE int
 *      #define TYPED(NAME) Int ## NAME
 *      #include "vec.h"
 *
 *      opt_IntVec vec_opt = Int_vec_new();
 *
 *      if (!vec_opt.present) {
 *          // handle error
 *          ...
 *      }
 *
 *      IntVec vec = vec_opt.item;
 *      Int_vec_push(&vec, 1);
 *      Int_vec_push(&vec, 2);
 *
 *      // Vector elements can be directly accessed through the `v` struct member
 *      int two = vec.v[1];
 *      long length = vec.length;
 *      int two_again = Int_vec_pop(&vec);
 *      ...
 * 
 * For a vector of ints, it is better to include structures.h and use the specialization there
 * than to specialize Vec yourself as in this example. You should specialize the data structure
 * yourself when you need to use it with a custom data type. See structures.h for some detailed
 * examples.
 *
 * The vector is first created with a nonzero capacity. Whenever a call to `vec_push` would
 * add an item that exceeds the vector's capacity, the vector is reallocated with 50% more
 * space. The vector only shrinks when `vec_shrink` is called! It is your responsibility
 * to ensure that you aren't needlessly eating up memory. Because the vector is allocated on
 * the heap, the associated `vec_free` function needs to be called when you're done with the
 * vector.
 */
#ifdef TYPE

#include "utils.h"
#include "errors.h"
#include "types.h"

#ifndef TYPED
#define TYPED(NAME) NAME
#endif

#ifndef _LIB_VEC_H_
#define _LIB_VEC_H_

#define INITIAL_VEC_CAPACITY 100

#endif

typedef struct {
    TYPE *v;

    long capacity;
    long length;
} TYPED(Vec);

DECL_OPTION(TYPED(Vec))

/**
 * Tries to create a vector with the default capacity. Will return an empty option with
 * an error code if the program cannot allocate enough memory.
 */
OPT(TYPED(Vec)) TYPED(_vec_new) ();

/**
 * Tries to create a vector with the given capacity. Will return an empty option with
 * an error code if the program cannot allocate enough memory or if the capacity is zero.
 */
OPT(TYPED(Vec)) TYPED(_vec_new_cap) (long capacity);

/**
 * Tries to append an item to the vector. May return a nonzero status code if the
 * vector needs to be reallocated and the program is out of memory.
 */
StatusCode TYPED(_vec_push) (TYPED(Vec) *vec, TYPE item);

/**
 * Tries to pop the last item in the vector. Returns an empty option if the vector
 * is empty.
 */
OPT(TYPE) TYPED(_vec_pop) (TYPED(Vec) *vec);

/**
 * Tries to shrink the capacity of the vector to match its length. May return an out of memory
 * error, indicating that the call to `realloc` to shrink the allocated buffer somehow failed.
 */
StatusCode TYPED(_vec_shrink) (TYPED(Vec) *vec);

/**
 * Frees the memory held by the vector. This must be called when you're done with the vector.
 * The vector struct becomes unusable after this call and should be dropped or freed as
 * well.
 */
void TYPED(_vec_free) (TYPED(Vec) *vec);


// IMPLEMENTATION


OPT(TYPED(Vec)) TYPED(_vec_new) () {
    TYPE *v = malloc(sizeof *v * INITIAL_VEC_CAPACITY);

    if (v == NULL) {
        return OPT_NONE_ERR(TYPED(Vec), ERROR_OUT_OF_MEM);
    }

    TYPED(Vec) out = {
        .v = v,
        .capacity = INITIAL_VEC_CAPACITY,
        .length = 0
    };

    return OPT_SOME(TYPED(Vec), out);
}

OPT(TYPED(Vec)) TYPED(_vec_new_cap) (long capacity) {
    if (!capacity) {
        return OPT_NONE_ERR(TYPED(Vec), ERROR_BAD_ARG);
    }
    
    TYPE *v = malloc(sizeof *v * capacity);

    if (v == NULL) {
       return OPT_NONE_ERR(TYPED(Vec), ERROR_OUT_OF_MEM);
    }

    TYPED(Vec) out = {
        .v = v,
        .capacity = capacity,
        .length = 0
    };

    return OPT_SOME(TYPED(Vec), out);
}

StatusCode TYPED(_vec_push) (TYPED(Vec) *vec, TYPE item) {
    if (vec->length == vec->capacity) {
        // If the vec is full, we will try to increase its capacity by 50%
        const long new_capacity = 1.5 * vec->capacity;
        TYPE *new_v = realloc(vec->v, sizeof *new_v * new_capacity);

        if (new_v == NULL) {
            return ERROR_OUT_OF_MEM;
        }

        vec->v = new_v;
        vec->capacity = new_capacity;
    }

    vec->v[vec->length++] = item;

    return STATUS_OK;
}

OPT(TYPE) TYPED(_vec_pop) (TYPED(Vec) *vec) {
    if (!vec->length) {
        return OPT_NONE(TYPE);
    }
    
    const TYPE item = vec->v[--(vec->length)];

    return OPT_SOME(TYPE, item);
}

StatusCode TYPED(_vec_shrink) (TYPED(Vec) *vec) {
    TYPE *new_v = realloc(vec->v, sizeof *new_v * vec->length);

    if (new_v == NULL) {
        return ERROR_OUT_OF_MEM;
    }

    vec->v = new_v;
    vec->capacity = vec->length;

    return STATUS_OK;
}

void TYPED(_vec_free) (TYPED(Vec) *vec) {
    free(vec->v);
}

#endif
