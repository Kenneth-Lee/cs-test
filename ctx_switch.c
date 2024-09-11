/**
 * Copyright by Kenneth Lee, 2024. All Rights Reserved.
 * 
 * 这个程序创建一组进程，测试他们上下文切换会产生的成本（用perf测试），原理如下：
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

#define BOUND_CPU 2		// 避开主核（包括它的超线程）
#define NUM_PROCESSES 3
#define LOOP_COUNT 1000

void child_logic(int id);

struct child_struc {
	pid_t pid;            // 本进程的pid
	int pipefd[2];        // 本进程对下一个进程的通讯pipe
} children[NUM_PROCESSES];
int current_p = 0;            // 当前初始化的进程，0是主进程。
#define print(fmt, ...) printf("%d: " fmt, current_p, ##__VA_ARGS__)

#define SYS_ASSERT(cond, msg) while(0) \
	if (!(cond)) { \
		perror(msg); \
		exit(EXIT_FAILURE); \
	}

// 主工作循环：等待通讯，循环，激活下一级
void workloop(int wfd, int rfd) {
	char buf[128];
	int ret;

	while(1) {
		print("wait for read on fd %d...\n", rfd);
		ret = read(rfd, buf, sizeof(buf));
		SYS_ASSERT(ret > 0, "read");
		print("read %s\n", buf);

		for (int i = 0; i < LOOP_COUNT; i++) ;

		print("kick next fd=%d\n", wfd);
		ret = write(wfd, "go", 3);
		SYS_ASSERT(ret > 0, "write2");
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
	if (current_p == NUM_PROCESSES-1) {
		// assert(NUM_PROCESSES>1)
		workloop(children[NUM_PROCESSES-1].pipefd[1], children[current_p-1].pipefd[0]);
		return;
	}

	// 创建和下一级通讯的pipe，0读，1写
	ret = pipe2(children[current_p].pipefd, O_DIRECT);
	SYS_ASSERT(ret>=0, "pipe2");
	print("create pipe pair: %d, %d\n", children[current_p].pipefd[0],children[current_p].pipefd[1]);

	// 创建子进程
	pid = fork(); 
	SYS_ASSERT(ret!=-1, "fork");

	if (pid) {
		if (current_p == 0) {
			// 第一个进程，激活一下，启动整个循环
			print("kick write to fd %d\n", children[current_p].pipefd[0]);
			ret = write(children[current_p].pipefd[1], "go", 3);
			SYS_ASSERT(ret > 0, "write");
			workloop(children[current_p].pipefd[1], children[NUM_PROCESSES-1].pipefd[0]);
		} else 
			workloop(children[current_p].pipefd[1], children[current_p-1].pipefd[0]);
	} else {
		// 子进程
		current_p++;
		work();             // 尾递归
	}
}

int main() {
	int ret;
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(2, &cpuset);

	// 绑定主进程
	ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
	SYS_ASSERT(ret >= 0, "sched_setaffinity");

	// 创建最后一个进程的pipe，以便可以loop回进程上
	ret = pipe2(children[NUM_PROCESSES-1].pipefd, O_DIRECT);
	SYS_ASSERT(ret>=0, "pipe init");
	print("create pipe pair for task %d: %d, %d\n", NUM_PROCESSES-1,
	      children[NUM_PROCESSES-1].pipefd[0],children[NUM_PROCESSES-1].pipefd[1]);

	work();

	return EXIT_SUCCESS;
}
