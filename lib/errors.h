#ifndef _LIB_ERRORS_H_
#define _LIB_ERRORS_H_
#include <stdio.h>

#define STATUS_OK 0
#define ERROR_OUT_OF_MEM 10
#define ERROR_BAD_ARG 20

typedef unsigned short StatusCode;

void print_error(const StatusCode error_code) {
    switch (error_code) {
        case STATUS_OK: {
            printf("OK");
            return;
        }
        case ERROR_OUT_OF_MEM: {
            printf("Out of memory");
            return;
        }
        case ERROR_BAD_ARG: {
            printf("Bad argument");
            return;
        }
        default: {
            printf("Unrecognized error code: %d", error_code);
            return;
        }
    }
}

#endif
