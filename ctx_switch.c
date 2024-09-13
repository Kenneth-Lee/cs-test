/**
 * Copyright by Kenneth Lee, 2024. All Rights Reserved.
 * 
 * 这个程序创建一组进程，测试他们上下文切换会产生的成本（用perf运行本程序的
 * WorkLoad测试），原理如下：
 *
 * 启动一个进程，先绑定CPU（绑定CPU会继承），然后创建一个同步pipe，创建一个子进
 * 程，子进程再创建一个同步pipe，创建下一个子进程，如此类推，最后一个子进程再和
 * 第一个进程通讯。之后每个进程循环一段时间就pipe下一个进程运行，就可以形成一个
 * 可控的调度循环了。
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include "misc.h"

#define LOOP_UNIT 1024
static int cfg_loop = 10;
static int cfg_cpu = 2; 		// 默认避开主核和主核的超线程
static int cfg_max = 1000;              // 切换次数到达结束，0表示永远循环
static int cfg_num = 3;                 // 默认切换进程数量。

void child_logic(int id);

struct child_struc {
	pid_t pid;            // 本进程的pid
	int pipefd[2];        // 本进程对下一个进程的通讯pipe
} *children;
int current_p = 0;            // 当前初始化的进程，0是主进程。
//#define print(fmt, ...) printf("%d: " fmt, current_p, ##__VA_ARGS__)
#define print(fmt, ...)

// 主工作循环：等待通讯，循环，激活下一级
void workloop(int wfd, int rfd) {
	char buf[128];
	int ret;
	int count = 0;

	while(1) {
		print("wait for read on fd %d...\n", rfd);
		ret = read(rfd, buf, sizeof(buf));
		DIE_IF(ret <= 0, "read");
		print("read %s\n", buf);

		for (int i = 0; i < cfg_loop*LOOP_UNIT; i++) ;

		print("kick next fd=%d\n", wfd);
		ret = write(wfd, "go", 3);
		DIE_IF(ret <= 0, "write2");

		if (cfg_max>0) {
			if (++count>cfg_max) {
				printf("Test done (loop %d)\n", count);
				exit(EXIT_SUCCESS);
			}
		}
	}
}

// 每个进程的行为
void work() {
	pid_t pid;
	int ret;

	// 当前pid
	children[current_p].pid = getpid();
	print("start pid %d\n", children[current_p].pid);

	// 这是最后一个进程了，pipe已经提前创建了，loop到第一个进程上。
	if (current_p == cfg_num-1) {
		workloop(children[cfg_num-1].pipefd[1], children[current_p-1].pipefd[0]);
		return;
	}

	// 创建和下一级通讯的pipe，0读，1写
	ret = pipe2(children[current_p].pipefd, O_DIRECT);
	DIE_IF(ret<0, "pipe2");
	print("create pipe pair: %d, %d\n", children[current_p].pipefd[0],children[current_p].pipefd[1]);

	// 创建子进程
	pid = fork(); 
	DIE_IF(ret==-1, "fork");

	if (pid) {
		if (current_p == 0) {
			// 第一个进程，激活一下，启动整个循环
			print("kick write to fd %d\n", children[current_p].pipefd[0]);
			ret = write(children[current_p].pipefd[1], "go", 3);
			DIE_IF(ret <= 0, "write");
			workloop(children[current_p].pipefd[1], children[cfg_num-1].pipefd[0]);
		} else 
			workloop(children[current_p].pipefd[1], children[current_p-1].pipefd[0]);
	} else {
		// 子进程
		current_p++;
		work();             // 尾递归
	}
}

void parse_opt(int argc, char * argv[]) {
	int opt;

	while((opt=getopt(argc, argv, "l:c:M:n:" MISC_OPT)) != -1) {
		switch(opt) {
			PARSE_MISC_OPT;
			PARSE_ARG_I('l', cfg_loop);
			PARSE_ARG_I('c', cfg_cpu);
			PARSE_ARG_I('M', cfg_max);
			PARSE_ARG_I('n', cfg_num);
			default:
				fprintf(stderr,
					"usage: %s [-l loop] [-c bound_cpu] [-M max_switch]"
					" [-n process_num]\n",
					argv[0]);
				exit(EXIT_SUCCESS);
		}
	}
}

int main(int argc, char *argv[]) {
	int ret;
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cfg_cpu, &cpuset);

	parse_opt(argc, argv);

	if (cfg_num < 2) {
		fprintf(stderr, "must assign at lease 2 threads (now %d)\n", cfg_num);
		exit(EXIT_SUCCESS);
	}

	// 绑定主进程
	ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
	DIE_IF(ret < 0, "sched_setaffinity");

	children = (struct child_struc *)calloc(cfg_num, sizeof(*children));
	DIE_IF(!children, "calloc");

	// 创建最后一个进程的pipe，以便可以loop回主进程上
	ret = pipe2(children[cfg_num-1].pipefd, O_DIRECT);
	DIE_IF(ret < 0, "pipe init");
	print("create pipe pair for task %d: %d, %d\n", cfg_num-1,
	      children[cfg_num-1].pipefd[0], children[cfg_num-1].pipefd[1]);

	work();

	free(children);
	return EXIT_SUCCESS;
}
