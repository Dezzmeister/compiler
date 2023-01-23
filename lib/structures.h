/**
 * Common data structures specialized for primitive types, and char*. The data structures covered by
 * this header file are:
 *
 *      LinkedList
 *      Vec
 * 
 * Each data structure has a prefix indicating the type of its items. For example, UIntLinkedList is a
 * LinkedList containing unsigned ints. In general, these are the prefixes used to indicate different
 * typed data structures (prefixes on the left, typenames on the right):
 *
 *      Char          char
 *      Short         short
 *      Int           int
 *      Long          long
 *      Fp            float
 *      Dp            double
 *      UChar         unsigned char
 *      UShort        unsigned short
 *      UInt          unsigned int
 *      ULong         unsigned long
 *      Str           char*
 *
 * The best thing is generally to include this header file somewhere in your code and get access to these
 * data structures. Each data structure also comes with an option type wrapping the data structure. The
 * option type starts with the prefix `opt_` - so to get an optional container for an IntLinkedList (linked list
 * of signed ints), the typename would be `opt_IntLinkedList`. See utils.h for a more detailed description
 * of the option macro `DECL_OPTION` and the option typedef.
 *
 * To define your own specializations of these data structures for a custom type, first make sure that
 * you've defined an option wrapper for the type:
 *      
 *      #include "utils.h"
 *
 *      DECL_OPTION(Type)
 *
 * then define TYPE to be the name of your custom type, and define the TYPED(NAME) macro to take an identifier
 * and prepend a prefix according to your custom type. For example, if defining data structures for a custom
 * `Building` struct, you might do the following:
 *
 *      #define TYPE Building
 *      #define TYPED(NAME) Bldg ## NAME
 *
 * If the type name has spaces, you will need to alias it with a typedef. Otherwise, the generated identifiers
 * will have spaces and will be syntactically invalid. See types.h for some aliases for common types.
 * After defining TYPE and TYPED, include the data structures you're specializing, and undefine TYPE and TYPED
 * when you're done. The whole block of code to create specialized "Building" data structures might look like
 * this:
 *
 *      #include "utils.h"
 *
 *      typedef struct {...} Building;
 *
 *      DECL_OPTION(Building)
 *
 *      #define TYPE Building
 *      #define TYPED(NAME) Bldg ## NAME
 *      #include "list.h"
 *      #include "vec.h"
 *      #undef TYPED
 *      #undef TYPE
 *
 * Including a data structure's header file (list.h, vec.h) will also generate an option type wrapping the
 * data structure. At the end of this code, you will have the following types available:
 *      
 *      opt_Building
 *      BldgLinkedList
 *      BldgVec
 *      opt_BldgLinkedList
 *      opt_BldgVec
 *
 * The types prefixed with `opt_` are structs representing optional values of the data structures (i.e., they
 * may contain a data structure, or they may not). See utils.h and DECL_OPTION for more info.
 */
#ifndef _LIB_STRUCTURES_H_
#define _LIB_STRUCTURES_H_
#include "types.h"

#define TYPE char
#define TYPED(NAME) Char ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE short
#define TYPED(NAME) Short ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE


#define TYPE int
#define TYPED(NAME) Int ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE long
#define TYPED(NAME) Long ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE float
#define TYPED(NAME) Fp ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE double
#define TYPED(NAME) Dp ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE uchar
#define TYPED(NAME) UChar ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE ushort
#define TYPED(NAME) UShort ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE uint
#define TYPED(NAME) UInt ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE ulong
#define TYPED(NAME) ULong ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#define TYPE char_ptr
#define TYPED(NAME) Str ## NAME
#include "list.h"
#include "vec.h"
#undef TYPED
#undef TYPE

#endif
