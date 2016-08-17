#ifndef __TASK_H__
#define __TASK_H__

#include <pthread.h>

struct task {
	pthread_t thread;
	void * arg;
};

struct task * create_task(void *(*routin)(void *), void * arg);
void join_task(struct task * tsk);

#endif
