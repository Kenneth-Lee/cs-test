#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>

#include "misc.h"
#include "task.h"
#include "cal.h"

int cfg_p = 99;
int cfg_s = 1;
int cfg_q = 4;

static long time1=0, time2=0;

static pthread_spinlock_t lock;

struct sstat {
	int count;
	int latency;
}*stat;

#define PARSE_ARG_I(key, arg) case key: arg = atoi(optarg); break
#define PARSE_ARG(key, arg) case key: arg = 1; break
void parse_opt(int argc, char * argv[]) {
	int opt;

	while((opt=getopt(argc, argv, "p:s:q:")) != -1) {
		switch(opt) {
			PARSE_ARG_I('p', cfg_p);
			PARSE_ARG_I('s', cfg_s);
			PARSE_ARG_I('q', cfg_q);
			default:
				fprintf(stderr, "usage: %s [-p p] [-s s] [-q q]",
						argv[0]);
				exit(0);

		}
	}
}

void * thread_routin(void * arg) {
	struct task * tsk = arg;
	int i = (int)(intptr_t)tsk->arg;
	long time1, time2;

	while(1) {
		time1 = get_timestamp();
		heavy_cal(10, cfg_p);
		pthread_spin_lock(&lock);
		heavy_cal(20, cfg_s);
		pthread_spin_unlock(&lock);
		time2 = get_timestamp();
		stat[i].count++;
		stat[i].latency +=time2-time1;
	}
}

void int_handler(int signum) {
	int i;
	int sum=0;
	int lsum=0;

	time2=get_timestamp();

	printf("exit\nstat:\n");
	for(i=0; i<cfg_q; i++) {
		DIE_IF(!stat[i].count, "no count");
		printf("stat[%d]=%d,%d\n", i, stat[i].count, stat[i].latency/stat[i].count);
		sum+=stat[i].count;
		lsum+=stat[i].latency;
	}
	printf("sum=%d, ave=%ldk/s, latency=%dms\n", sum, sum/((time2-time1)/1000), lsum/sum);

	pthread_spin_destroy(&lock);

	exit(signum);
}

int main(int argc, char *argv[]) {
	struct task **tasks;
	int i;
	int ret;

	cfg_ncpu = get_ncpu();

	parse_opt(argc, argv);

	printf("amdahl(p=%d, s=%d, q=%d)\n", cfg_p, cfg_s, cfg_q);

	DIE_IF(cfg_q>cfg_ncpu, "q(%d)>cpus(%d)\n", cfg_q, cfg_ncpu);

	tasks = (struct task **)malloc(cfg_q * sizeof(struct task *));
	DIE_IF(!tasks, "malloc");

	ret = pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
	DIE_IF(ret, "spin_init");

	stat = (struct sstat *)calloc(cfg_q, sizeof(struct sstat));
	DIE_IF(!stat, "calloc");

	signal(SIGINT, int_handler);

	time1 = get_timestamp();
	for(i=0; i<cfg_q; i++)
		tasks[i] = create_task_on_cpu(thread_routin, (void *)(intptr_t)i, i);
	for(i=0; i<cfg_q; i++)
		join_task(tasks[i]);

	free(stat);

	return EXIT_SUCCESS;
}
