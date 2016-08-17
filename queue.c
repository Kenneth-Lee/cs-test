#include <pthread.h>

#include "misc.h"

#define QUEUE_SIZE 100
#define MOVE(var) var = (var+1)%QUEUE_SIZE

int queue[QUEUE_SIZE];
int head=tail=0

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void en_q(int data) {

	ret = pthread_mutex_lock(&lock);
	DIE_IF(ret, "lock");

	while((head+1)%QUEUE_SIZE == tail) {
		pthread_cond_wait(&cond);
	}

	queue[head] = data;
	MOVE(head);

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
		pthread_cond_wait(&cond);
	}

	data = queue[tail];
	MOVE(tail);

	ret = pthread_cond_signal(&cond);
	DIE_IF(ret, "cond_signal");

	pthread_mutex_unlock(&lock);
	DIE_IF(ret, "unlock");
}
