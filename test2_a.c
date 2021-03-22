#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"
#include "sys/sem.h"
#include "pthread.h"

int selled;//已售票
int total;//总数
int semid;//信号灯id
void thread1(void);
void thread2(void);
void V(int semid, int index);
void P(int semid, int index);

union semun {
	int val;

	struct semid_ds* buf;

	unsigned short* array;

};

int main(void) {
	semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);//创建一个信号灯
	union semun arg;
	arg.val = 1;
	semctl(semid, 0, SETVAL, arg);
	selled = 0;
	total = 10;
	pthread_t id1;//子线程id
	pthread_t id2;
	int ret1, ret2;

	ret1 = pthread_create(&id1, NULL, (void*)thread1, NULL);
	ret2 = pthread_create(&id2, NULL, (void*)thread2, NULL);

	if (ret1 != 0 || ret2 != 0) {
		printf("thread created failed");
		exit(0);
	}

	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	printf("%d tickets are totally sold\n", selled);
	return 0;
}

void thread1(void) {
	printf("thread 1 is created\n");
	int sell1 = 1;//线程1卖的票数
	while (1) {//开始卖票
		P(semid, 0);//访问信号灯
		if (selled == total) {//卖完了
			printf("all tickets are sold\n");
			V(semid, 0);
			break;
		}
		printf("thread 1 sells %d tickets \n", selled);
		sell1++;
		selled++;
		V(semid, 0);
		sleep(1);
	}
	printf("thread 1 has sold %d tickets\n", sell1 - 1);
	printf("thread 1 exited\n");
	pthread_exit(0);
}

void thread2(void) {
	printf("thread 2 is created\n");
	int sell2 = 1;//线程1卖的票数
	while (1) {//开始卖票
		P(semid, 0);//访问信号灯
		if (selled == total) {//卖完了
			printf("all tickets are sold\n");
			V(semid, 0);
			break;
		}
		printf("thread 2 sells %d tickets \n", selled);
		sell2++;
		selled++;
		V(semid, 0);
		sleep(1);
	}
	printf("thread 2 has sold %d tickets\n", sell2 - 1);
	printf("thread 2 exited\n");
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


