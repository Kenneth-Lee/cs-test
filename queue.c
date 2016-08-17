#include <pthread.h>
#include <signal.h>

#include "misc.h"

DEF_STAT_VAR(full_hit);
DEF_STAT_VAR(empty_hit);
DEF_STAT_VAR(enq);
DEF_STAT_VAR(deq);
DEF_STAT_MAX(qlen);

#define QUEUE_SIZE 1000
#define MOVE(var) var = (var+1)%QUEUE_SIZE

int queue[QUEUE_SIZE];
int head=0, tail=0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void en_q(int data) {
	int ret;
	int qlen;

	ret = pthread_mutex_lock(&lock);
	DIE_IF(ret, "lock");

	while((head+1)%QUEUE_SIZE == tail) {
		STAT(full_hit);
		pthread_cond_wait(&cond, &lock);
	}

	queue[head] = data;
	MOVE(head);
	STAT(enq);

	qlen=head-tail;
	if(qlen<0)
		qlen+=QUEUE_SIZE;
	STAT_MAX(qlen, qlen);

	ret = pthread_cond_signal(&cond);
	DIE_IF(ret, "cond_signal");

	ret = pthread_mutex_unlock(&lock);
	DIE_IF(ret, "unlock");
}

int de_q(void) {
	int ret, data;

	ret = pthread_mutex_lock(&lock);
	DIE_IF(ret, "lock");

	while(head == tail) {
		STAT(empty_hit);
		pthread_cond_wait(&cond, &lock);
	}

	data = queue[tail];
	MOVE(tail);
	STAT(deq);

	ret = pthread_cond_signal(&cond);
	DIE_IF(ret, "cond_signal");

	pthread_mutex_unlock(&lock);
	DIE_IF(ret, "unlock");

	return data;
}

void print_q_stat(void) {
	PRINT_STAT(full_hit);
	PRINT_STAT(empty_hit);
	PRINT_STAT(enq);
	PRINT_STAT(deq);
	PRINT_STAT_MAX(qlen);
}
