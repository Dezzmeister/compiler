/**
 * Type aliases for some primitive types and option containers for them.
 * See utils.h for an explanation of the `DECL_OPTION` macro.
 *
 * The macros for specializing generic data structures don't work when the type
 * argument has spaces. For example, you couldn't create a LinkedList with a type
 * argument of "unsigned long". The typedefs in this file are needed to provide some
 * aliases for types that contain spaces. 
 *
 * Before specializing a data structure for a given type, you need to make sure that you
 * have generated an option container for that type with `DECL_OPTION`. This file also
 * sontains some `DECL_OPTION` invocations for primitive types, which are included by
 * structures.h to generate specialized data structures.
 */
#ifndef _LIB_TYPES_H_
#define _LIB_TYPES_H_
#include "utils.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef char *char_ptr;
typedef short *short_ptr;
typedef int *int_ptr;
typedef long *long_ptr;
typedef float *float_ptr;
typedef double *double_ptr;

typedef unsigned char *uchar_ptr;
typedef unsigned short *ushort_ptr;
typedef unsigned int *uint_ptr;
typedef unsigned long *ulong_ptr;

DECL_OPTION(char)
DECL_OPTION(short)
DECL_OPTION(int)
DECL_OPTION(long)
DECL_OPTION(float)
DECL_OPTION(double)

DECL_OPTION(uchar)
DECL_OPTION(ushort)
DECL_OPTION(uint)
DECL_OPTION(ulong)

DECL_OPTION(char_ptr)
DECL_OPTION(short_ptr)
DECL_OPTION(int_ptr)
DECL_OPTION(long_ptr)
DECL_OPTION(float_ptr)
DECL_OPTION(double_ptr)

DECL_OPTION(uchar_ptr)
DECL_OPTION(ushort_ptr)
DECL_OPTION(uint_ptr)
DECL_OPTION(ulong_ptr)

#endif
