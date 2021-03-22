#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "signal.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/sem.h"

#define SHMKEY1 1111
#define SHMKEY2 2111
#define SHMKEY3 3111
#define SHMKEY4 4111

int semid; //信号灯id

int shmid1, shmid2, shmid3, shmid4;	 //四个共享储存
char *addr1, *addr2, *addr3, *addr4; //共享缓冲区地址

void V(int semid, int index);
void P(int semid, int index);
void GET();
void PUT();
int pid_p, pid_g;

union semun
{
	int val;

	struct semid_ds *buf;

	unsigned short *array;
};

int main()
{
	shmid1 = shmget(SHMKEY1, 100 * sizeof(char), 0666 | IPC_CREAT);
	shmid2 = shmget(SHMKEY2, 100 * sizeof(char), 0666 | IPC_CREAT);
	shmid3 = shmget(SHMKEY3, 100 * sizeof(char), 0666 | IPC_CREAT);
	shmid4 = shmget(SHMKEY4, 100 * sizeof(char), 0666 | IPC_CREAT);

	semid = semget((key_t)13, 3, IPC_CREAT | 0666); //创建信号灯
	union semun arg1, arg2,arg3;
	arg1.val = 4;
	semctl(semid, 0, SETVAL, arg1); //空缓冲区数量
	arg2.val = 0;
	semctl(semid, 1, SETVAL, arg2); //可读缓冲区数量
	arg3.val = 0;
	semctl(semid, 2, SETVAL, arg3); //可读缓冲区数量

	if ((pid_g = fork()) == 0)
	{ //创建get
		puts("get created\n");
		execv("./get", NULL);
	}
	else if ((pid_p = fork()) == 0)
	{ //创建put
		puts("put created\n");
		execv("./put", NULL);
	}

	wait();
	wait();
	printf("delete shm\n");
	shmctl(shmid1, IPC_RMID, 0); /*撤消共享存储区，归还资源*/
	shmctl(shmid2, IPC_RMID, 0);
	shmctl(shmid3, IPC_RMID, 0);
	shmctl(shmid4, IPC_RMID, 0);
	printf("delete sem\n");
	semctl(semid, IPC_RMID, 0);
	return 0;
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


