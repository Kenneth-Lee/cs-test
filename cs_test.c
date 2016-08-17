#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "misc.h"
#include "task.h"
#include "queue.h"

void * pro_routin(void * arg) {
	struct task * tsk = arg;
	int ret;

	srand((intptr_t)tsk->arg);

	while(1) {
		ret = rand();
		en_q(ret);
		sleep(0);
	}
}

void * con_routin(void * arg) {
	int ret;

	ret = 0;
	while(1) {
		ret *= de_q();
		sleep(0);
	}
}

void int_handler(int signum) {
	printf("exit\n");
	exit(signum);
}

int main(void) {
	struct task *pro, *con;

	signal(SIGINT, int_handler);

	pro = create_task(pro_routin, (void *)(intptr_t)rand());
	con = create_task(con_routin, NULL);

	join_task(pro);
	join_task(con);

	return EXIT_SUCCESS;
}
