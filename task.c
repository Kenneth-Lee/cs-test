/**
 * Copyright by Kenneth Lee 2016
 */
#include <pthread.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include "misc.h"
#include "task.h"

int cfg_ncpu = 1;

int get_ncpu(void) {
	return sysconf(_SC_NPROCESSORS_ONLN);
}

struct task * create_task_on_cpu(void *(*routin)(void *), void *arg, int cpu) {
	struct task *tsk;
	cpu_set_t *cpuset;
	int cpuset_sz;
	int ret;


	tsk = (struct task *)malloc(sizeof(*tsk));
	DIE_IF(!tsk, "malloc");

	tsk->arg = arg;
	
	ret = pthread_attr_init(&tsk->attr);
	if(cpu>=0) {
		cpuset = CPU_ALLOC(cfg_ncpu);
		cpuset_sz = CPU_ALLOC_SIZE(cfg_ncpu);
		DIE_IF(!cpuset, "cpuset alloc");
		CPU_ZERO_S(cpuset_sz, cpuset);
		CPU_SET_S(cpu, cpuset_sz, cpuset);
		pthread_attr_setaffinity_np(&tsk->attr, cpuset_sz, cpuset);
		CPU_FREE(cpuset);
	}
	ret = pthread_create(&tsk->thread, &tsk->attr, routin, (void *)tsk);
	DIE_IF(ret, "pthread_create");


	return tsk;
}

void join_task(struct task * tsk) {
	pthread_join(tsk->thread, NULL);
	pthread_attr_destroy(&tsk->attr);
	free(tsk);
}

struct task_set * create_task_set(int num) {
	struct task_set * ts = malloc(sizeof(struct task_set));
	DIE_IF(!ts, "malloc");
	ts->num = num;
	ts->tasks = (struct task **)malloc(num * sizeof(struct task *));
	DIE_IF(!ts->tasks, "malloc");

	return ts;
}

void join_task_set(struct task_set *ts) {
	int i;

	for(i=0; i<ts->num; i++)
		join_task(ts->tasks[i]);

	free(ts);
}
