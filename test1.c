#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"

void myfunc(int sig_no);
void myfunc1(int sig_no);
void myfunc2(int sig_no);
int pipefd[2];

pid_t pid1, pid2;


int main() {
	pipe(pipefd);//创建无名管道
	signal(SIGINT, myfunc);//设置信号SIGINT处理；

	if ((pid1 = fork()) == 0) {
		printf("child 1 is created\n");
		child1();
	}
	else
		if ((pid2 = fork()) == 0) {
			printf("child 2 is created\n");
			child2();
		}
		else {
		       close(pipefd[0]);//关闭读端
			for (int x = 1; x < 51; x++) {			 
				write(pipefd[1], "aaaaaaaaaa", 10);
				sleep(1);
				printf("I send you %d times\n", x);
			}
			close(pipefd[1]);//关闭
			int p1, p2;
			wait(&p1);
			wait(&p2);
			printf("parent process exited\n");
			return 0;
		}

}

void myfunc(int sig_no) {
	int n1, n2;
	kill(pid1, SIGUSR1);
	kill(pid2, SIGUSR2);
	wait(&n1);
	wait(&n2);
	printf("Parent Process is killed\n");
	exit(0);
}


void child1(void) {
	signal(SIGINT, 1);//设置信号SIGINT处理；
	signal(SIGUSR1, myfunc1);//设置信号SIGUSR1处理；
	int i = 0;
	int rsta;
	while (1) {
		char buf[10] = { 0 };
		close(pipefd[1]);
		while(read(pipefd[0], buf, 10)>0){
			printf("child 1 %d times received : %s\n", i, buf);
			i++;
		}
			break;
		}
	close(pipefd[0]);
	printf("child 1 process exited\n");
	exit(0);
}


void child2(void) {
	signal(SIGINT, 1);//设置信号SIGINT处理；
	signal(SIGUSR2, myfunc2);//设置信号SIGUSR1处理；
	int i = 0;
	int rsta;
	while (1) {
	       close(pipefd[1]);
	       	char buf[10] = { 0 };
		while(read(pipefd[0], buf, 10)>0) {
			printf("child 2 %d times received : %s\n", i, buf);
			i++;
		}
			break;
}
	close(pipefd[0]);
	printf("child 2 process exited\n");
	exit(0);
}



void myfunc1(int sig_no) {
	//close(pipefd[0]);
	printf("Child Process 1 is Killed by Parent!\n");
	exit(0);
}

void myfunc2(int sig_no) {
	//close(pipefd[0]);
	printf("Child Process 2 is Killed by Parent!\n");
	exit(0);
}
