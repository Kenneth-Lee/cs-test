#include <pthread.h>
#include <assert.h>
#include "mcs_spinlock.h"

static pthread_spinlock_t plock;
static struct mcs_spinlock *pbase=NULL;
#define aenter() pthread_spin_lock(&plock);
#define aexit() pthread_spin_unlock(&plock);

void mcs_spin_init(void) {
	pthread_spin_init(&plock, PTHREAD_PROCESS_PRIVATE);
}

void mcs_spin_lock(struct mcs_spinlock *node)
{
	struct mcs_spinlock *prev;

	node->locked = 0;
	node->next   = NULL;

	/* prev=xchg(&pbase, node) */
	aenter();
	prev = pbase;
	pbase = node;
	if (prev == NULL) {
		aexit();
		return; /* now lock is taken */
	}

	/* point the last taker to this cpu */
	prev->next = node; /* point to this lock block*/
	aexit();

	/* Wait until the lock holder passes the lock down. */
	while(!node->locked);
}

void mcs_spin_unlock(struct mcs_spinlock *node)
{
	struct mcs_spinlock *next;
       
	aenter();

	next = node->next;

	if(next) {
		assert(!next->locked);
		next->locked = 1;
	}else {
		assert(pbase==node);
		pbase = NULL;
	}
	aexit();
}
