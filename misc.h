#ifndef __MISC_H__
#define __MISC_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


#define DIE_IF(cond, fmt, ...) DIE_assert_func(__FILE__, __LINE__, !!(cond), fmt, ##__VA_ARGS__)

void DIE_assert_func(char * f, int line, int cond, const char *fmt, ...) {
        va_list args;

        va_start(args, fmt);
        if(!cond) {
                printf("DIE at %s:%i: ", f, line);
                vprintf(fmt, args);
                printf("\n");
                abort();
        }
        va_end(args);
}

#endif
