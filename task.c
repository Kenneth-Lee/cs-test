#include <pthread.h>
#include <stdlib.h>
#include "misc.h"
#include "task.h"

struct task * create_task(void *(*routin)(void *), void *arg) {
	struct task *tsk;
	int ret;

	tsk = (struct task *)malloc(sizeof(*tsk));
	DIE_IF(!tsk, "malloc");

	tsk->arg = arg;
	
	ret = pthread_create(&tsk->thread, NULL, routin, (void *)tsk);
	DIE_IF(ret, "pthread_create");

	return tsk;
}

void join_task(struct task * tsk) {
	pthread_join(tsk->thread, NULL);
	free(tsk);
}
