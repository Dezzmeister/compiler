/**
 * Miscellaneous code that has no better place.
 */
#ifndef _LIB_UTILS_H_
#define _LIB_UTILS_H_
#include <stdbool.h>
#include "errors.h"

/**
 * Expands to the typename for an option struct wrapping the given Type. Pairs with
 * `DECL_OPTION`.
 *
 * This and the following `OPT_...` macros can be used in generic code to refer to option
 * types or functions for a generic data structure. See vec.h or list.h for examples of this.
 */
#define OPT(Type) M_CONC(opt_, Type)

/**
 * Expands to the function that creates an empty option of type OPT(Type). This looks like
 * `opt_TYPENAME_none()`.
 */
#define OPT_NONE(Type) M_CONC(opt_, M_CONC(Type, _none()))

/**
 * Expands to the function that creates an option of type OPT(Type) with value (expr). This
 * looks like `opt_TYPENAME_some((expr))`.
 */
#define OPT_SOME(Type, expr) M_CONC(opt_, M_CONC(Type, _some((expr))))

/**
 * The preprocessor concatenates on '##' first, then expands macros. This causes trouble
 * when you try to invoke a macro that concatenates its argument, with an argument that is
 * itself another macro expansion. The usual workaround for this is a concatenation macro
 * or a few. In this case, use M_CONC to concatenate two terms where each term may be a nested
 * macro expansion.
 */
#define M_CONC(A, B) M_CONC_1(A, B)
#define M_CONC_1(A, B) M_CONC_2(A, B)
#define M_CONC_2(A, B) M_CONC_3(A, B)
#define M_CONC_3(A, B) M_CONC_4(A, B)
#define M_CONC_4(A, B) M_CONC_5(A, B)
#define M_CONC_5(A, B) A##B

/**
 * Declares an option type wrapping the given type name. Also declares two associated functions that
 * can be used to create objects of the option type. Example:
 *      #include "utils.h"
 *      
 *      DECL_OPTION(int)
 *
 *      void func() {
 *          opt_int has_none = opt_int_none();
 *          opt_int has_some = opt_int_some(5);
 *
 *          if (has_some.present) {
 *              int item = has_some.item;
 *              ...
 *          }
 *
 *          if (has_none.present) {
 *              // Unreachable
 *          }
 *          ...
 *      }
 *
 * In this example, `opt_int` is a type created by `DECL_OPTION`. The `DECL_OPTION` invocation expands
 * to the following (with whitespace added for readability):
 *      typedef struct {
 *          int item;
 *          bool present;
 *          StatusCode error;
 *      } opt_int;
 *      
 *      opt_int opt_int_none() {
 *          const opt_int out = {
 *              .present = false
 *          };
 *
 *          return out;
 *      }
 *
 *      opt_int opt_int_some(const int __arg) {
 *          const opt_int out = {
 *              .present = true,
 *              .item = __arg
 *          };
 *
 *          return out;
 *      }
 *
 *  The `error` field in the generated struct may be used to indicate an error code to the caller. For example,
 *  a constructor returning an optional data structure may set the error code to indicate that
 *  the program is out of memory. Because of the error field, the `present` field isn't really
 *  needed but it's there for readability and ease of use.
 */
#define DECL_OPTION(Type) \
    typedef struct { \
        Type item; \
        bool present; \
        StatusCode error; \
    } M_CONC(opt_, Type); \
    M_CONC(opt_, Type) M_CONC(opt_, M_CONC(Type, _none))() { \
        const M_CONC(opt_, Type) out = { \
            .present = false \
        }; \
        return out; \
    } \
    M_CONC(opt_, Type) M_CONC(opt_, M_CONC(Type, _some))(const Type __arg) { \
        const M_CONC(opt_, Type) out = { \
            .present = true, \
            .item = __arg \
        }; \
        return out; \
    } \

/**
 * GCC-specific code. Our project is coupled to GCC at this point but that can change. GCC provides some
 * nice compiler extensions and we're compiling on a linux machine with gcc so we might as well use them.
 */
#ifdef __GNUC__
#include <stdlib.h>
#include <string.h>

/**
 * Moves some data from the stack to the heap and evaluates to a pointer to the heap memory.
 * This relies on statement expressions and the typeof operator which are both GNU C extensions,
 * so it is guarded by #ifdef __GNUC__.
 *
 * https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
 * https://gcc.gnu.org/onlinedocs/gcc/Typeof.html
 *
 * By convention, the data structures in this folder expose constructors that create objects on the
 * stack. You can use this macro to construct them on the stack and then move them to the heap.
 * Here's an example:
 *      #include "structures.h"
 *      #include "utils.h"
 *
 *      void func() {
 *          IntLinkedList* heap_list = HEAP_ALLOC(Int_ll_new());
 *          ...
 *          Int_ll_free(heap_list); // Should be done whether or not list is in heap
 *          free(heap_list);
 *      }
 * The macro uses `__typeof__` to infer the type of the expression in `obj`, and it assigns to
 * `_obj` the result of the expression `obj`. The size of `_obj` is assigned to `_size`, and
 * `__typeof__` is used again to declare a pointer to the type of `obj`. The stack-allocated value
 * in `_obj` is moved to the heap with `memcpy` and the pointer from malloc is "returned." In a GCC
 * statement expression, the whole block evaluates to `expr`, where `expr;` is the last statement
 * in the block.
 */
#define HEAP_ALLOC(obj) \
    ({ __typeof__ (obj) _obj = (obj); \
       int _size = sizeof _obj; \
       __typeof__ (&_obj) _ptr = malloc(_size); \
       memcpy(_ptr, &_obj, _size); \
       _ptr; \
    })

/**
 * Expands to a statement expression. The statement expression evaluates to an option struct of type
 * OPT(Type), with an error code of `code`.
 */
#define OPT_NONE_ERR(Type, code) \
    ({ M_CONC(opt_, Type) __out = M_CONC(opt_, M_CONC(Type, _none))(); \
       __out.error = (code); \
       __out; \
    })

#endif // End of GCC-specific macros
#endif
