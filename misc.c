#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "misc.h"

void DIE_check_func(char * f, int line, int cond, const char *fmt, ...) {
        va_list args;

        va_start(args, fmt);
        if(cond) {
                printf("DIE at %s:%i: ", f, line);
                vprintf(fmt, args);
                printf("\n");
                abort();
        }
        va_end(args);
}
