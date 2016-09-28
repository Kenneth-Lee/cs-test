/**
 * Copyright by Kenneth Lee 2016
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>
#include "cal.h"
#include "misc.h"
#include "task.h"

int cfg_size=1024*1024;
int cfg_fill_num=1000;
int cfg_thread_num=4;
int cfg_cal_load=100;

static long time1=0, time2=0;
DEF_STAT_VAR(run);

void int_handler(int signum) {
	time2 = get_timestamp();
	verbose("exit\nstat:\n");
	PRINT_STAT(run);
	printf("break in %ldms\n", (time2-time1)/1000);
	exit(signum);

}

void parse_opt(int argc, char * argv[]) {
	int opt;

	while((opt=getopt(argc, argv, "s:n:q:c:")) != -1) {
		switch(opt) {
			PARSE_ARG_I('s', cfg_size);
			PARSE_ARG_I('n', cfg_fill_num);
			PARSE_ARG_I('q', cfg_thread_num);
			PARSE_ARG_I('c', cfg_cal_load);
			default:
				fprintf(stderr, "usage: %s [-s size (kb)] [-n fill_num] [-q thread_num] [-c cal_load]",
						argv[0]);
				exit(0);

		}
	}
}

void * thread_routin(void * arg) {
	init_rand();
	while(1) {
		random_malloc_free(cfg_size, cfg_fill_num);
		heavy_cal(10, cfg_cal_load);
		STAT(run);
	}
}

int main(int argc, char *argv[]) {
	int i;
	struct task_set * ts;

	parse_opt(argc, argv);
	DIE_IF(!cfg_size, "cfg_size");
	DIE_IF(!cfg_fill_num, "cfg_fill_num");
	DIE_IF(!cfg_thread_num, "cfg_thread_num");


	signal(SIGINT, int_handler);

	ts = create_task_set(cfg_thread_num);

	time1 = get_timestamp();

	for(i=0; i<cfg_thread_num; i++)
		ts->tasks[i] = create_task_on_cpu(thread_routin, NULL, i);

	join_task_set(ts);

	time2 = get_timestamp();

	PRINT_STAT(run);
	printf("finish in %ldus\n", time2-time1);

	return EXIT_SUCCESS;
}

