#include "signal.h"
#include "unistd.h"
#include "sys/sem.h"
#include "pthread.h"
#include "stdio.h"
#include <stdlib.h>

int a;
int i;
int semid;//信号灯id
void thread1(void);
void thread2(void);
void thread3(void);
void V(int semid, int index);
void P(int semid, int index);

union semun {
	int val;

	struct semid_ds* buf;

	unsigned short* array;

};

int main() {
	semid = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);//创建一个信号灯
	union semun arg;
	arg.val = 1;
	semctl(semid, 0, SETVAL, arg);//可写置1
	arg.val = 0;
	semctl(semid, 1, SETVAL, arg);//奇数可读置0
	arg.val = 0;
	semctl(semid, 2, SETVAL, arg);//偶数可读置0

	a = 0;//初始化
	i = 1;
	pthread_t id1, id2, id3;//子线程id

	int ret1, ret2, ret3;

	ret1 = pthread_create(&id1, NULL, (void*)thread1, NULL);
	ret2 = pthread_create(&id2, NULL, (void*)thread2, NULL);
	ret2 = pthread_create(&id3, NULL, (void*)thread3, NULL);

	if (ret1 != 0 || ret2 != 0 || ret3 != 0) {
		printf("thread created failed");
		exit(0);
	}
	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	pthread_join(id3, NULL);

	printf("calculate and print are all done\n");

	return 0;
}

void thread1(void) {
	printf("thread 1 for calculate is created\n");
	while (1) {
		P(semid, 0);//访问write信号灯
		if (i == 101) {//算完了
			printf("thread 1 calculate finish\n");
			V(semid, 1);
			break;
		}
		a += i;
		if (a % 2 == 1) {
			V(semid, 1);//奇数
		}
		else {
			V(semid, 2);//偶数
		}
		i++;
		//sleep(1);
	}
	printf("thread 1 exited\n");
	pthread_exit(0);
}

void thread2(void) {
	printf("thread 2 for odd print is created\n");
	while (1) {
		P(semid, 1);//访问奇数信号灯
		if (a == 5050) {
			printf("odd print finish\n");
			break;
		}
		printf("odd print is %d\n", a);
		V(semid, 0);
		//sleep(1);
	}
	printf("thread 2 exited\n");
	pthread_exit(0);
}

void thread3(void) {
	printf("thread 3 for even print is created\n");
	while (1) {
		P(semid, 2);//访问偶数信号灯
		if (a == 5050) {
			printf("even print is %d\n", a);
			V(semid, 0);
			printf("even print finish\n");
			break;
		}
		printf("even print is %d\n", a);
		V(semid, 0);
		//sleep(1);
	}
	printf("thread 3 exited\n");
	pthread_exit(0);
}


void P(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = -1;
	sem.sem_flg = 0;
	semop(semid, &sem, 1);
	return;
}

void V(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = 1;
	sem.sem_flg = 0;
	semop(semid, &sem, 1);
	return;
}

