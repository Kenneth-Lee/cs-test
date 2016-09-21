/**
 * Copyright by Kenneth Lee 2016
 */
#ifndef __CAL_H__
#define __CAL_H__

#define PAGE_SIZE (4*1024)

extern int heavy_cal(int var, int n_loop);
extern int heavy_mem_read(int npage);
extern int heavy_mem_write(int npage);
extern void random_malloc_free(int max_size, int fill_num);

#endif
