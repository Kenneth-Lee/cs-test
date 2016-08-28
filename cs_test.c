#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "misc.h"
#include "task.h"
#include "queue.h"

/* global parameters for the test */
static int n_pro = 1;
static int n_con = 1;
static int yield_method = 0;
static int n_p_cal = 100; 
static int n_c_cal = 100;
static void (*yield_method_f)(void);

void ym0(void) {
}

void ym1(void) {
	pthread_yield();
}

void ym2(void) {
	sleep(0);
}

int heavy_cal(int var, int n_loop) {
	int i;

	for(i=0; i<n_loop; i++) { 
		var = 1103515245 * var + 12345;
		var = 0x4190001473/var;
	}

	return var;
}

void * pro_routin(void * arg) {
	struct task * tsk = arg;
	int ret;

	srand((intptr_t)tsk->arg);

	while(1) {
		ret = heavy_cal(rand(), n_p_cal);
		en_q(ret);
		marker("yield here");
		yield_method_f();
	}
}

void * con_routin(void * arg) {
	int ret;

	ret = 0;
	while(1) {
		ret *= heavy_cal(de_q(), n_c_cal);
		yield_method_f();
	}
}

void int_handler(int signum) {
	printf("exit\nstat:");
	print_q_stat();
	exit(signum);
}

#define PARSE_ARG(i, arg) if(argc>(i)) arg = atoi(argv[i]);
int main(int argc, char * argv[]) {
	struct task **tasks;
	int i;

	PARSE_ARG(1, n_pro);
	PARSE_ARG(2, n_con);
	PARSE_ARG(3, yield_method);
	PARSE_ARG(4, n_p_cal);
	PARSE_ARG(5, n_c_cal);

	printf("cs n_pro=%d, n_con=%d, n_p_cal=%d, n_c_cal=%d\n",
			n_pro, n_con, n_p_cal, n_c_cal);

	if(n_pro<=0 || n_con<=0) {
		printf("usage: cs [n_pro] [n_con] [yield method] [n_p_cal] [n_c_cal]\n");
		return EXIT_SUCCESS;
	}

	switch(yield_method) {
		case 1:
			yield_method_f = ym1;
			break;
		case 2:
			yield_method_f = ym2;
			break;
		default:
			yield_method_f = ym0;
			break;
	}

	signal(SIGINT, int_handler);

	tasks = (struct task **)malloc((n_pro+n_con) * sizeof(struct task *));
	DIE_IF(!tasks, "malloc");

	for(i=0; i<n_pro; i++)
		tasks[i] = create_task(pro_routin, (void *)(intptr_t)rand());

	for(;i<n_pro+n_con;i++)
		tasks[i] = create_task(con_routin, NULL);

	for(i=0; i<n_pro+n_con; i++) 
		join_task(tasks[i]);

	return EXIT_SUCCESS;
}
