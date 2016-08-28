#ifndef __MISC_H__
#define __MISC_H__
#include <stdio.h>

extern void DIE_check_func(char * f, int line, int cond, const char *fmt, ...);
extern long get_timestamp(void);
extern void marker(const char *fmt, ...);

#define DIE_IF(cond, fmt, ...) DIE_check_func(__FILE__, __LINE__, !!(cond), fmt, ##__VA_ARGS__)

#define DEF_STAT_VAR(var) static volatile sig_atomic_t stat_##var = 0
#define STAT(var) stat_##var++
#define PRINT_STAT(var) printf("\tstat_" #var ":\t\t%d\n", stat_##var)

#define DEF_STAT_MAX(var) static volatile sig_atomic_t stat_max_##var = 0
#define STAT_MAX(var, v) if(v > stat_max_##var) stat_max_##var=v
#define PRINT_STAT_MAX(var) printf("\tstat_max_" #var ":\t\t%d\n", stat_max_##var)

#endif
