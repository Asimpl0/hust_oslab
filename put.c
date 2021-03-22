#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "signal.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/sem.h"
#include "string.h"
#define SHMKEY1 1111
#define SHMKEY2 2111
#define SHMKEY3 3111
#define SHMKEY4 4111
void V(int semid, int index);
void P(int semid, int index);
union semun
{
	int val;

	struct semid_ds *buf;

	unsigned short *array;
};
int main()
{
	char *buf[4];
	int shmid1, shmid2, shmid3, shmid4; //四个共享储存
	shmid1 = shmget(SHMKEY1, 100 * sizeof(char), 0666);
	shmid2 = shmget(SHMKEY2, 100 * sizeof(char), 0666);
	shmid3 = shmget(SHMKEY3, 100 * sizeof(char), 0666);
	shmid4 = shmget(SHMKEY4, 100 * sizeof(char), 0666);

	buf[0] = (char *)shmat(shmid1, 0, 0); /*获取首地址*/
	buf[1] = (char *)shmat(shmid2, 0, 0); /*获取首地址*/
	buf[2] = (char *)shmat(shmid3, 0, 0); /*获取首地址*/
	buf[3] = (char *)shmat(shmid4, 0, 0); /*获取首地址*/
	int semid;
	semid = semget((key_t)13, 3, IPC_CREAT | 0666); //创建信号灯

	FILE *fp;
	if ((fp = fopen("output.txt", "w")) == NULL)
	{
		printf("open file error\n");
		exit(0);
	}
	int i = 0;
	int d = 0;
	char c[100] = "EOF";
	while (1)
	{
		P(semid, 1);
		if (strcmp(buf[i], c) == 0)
		{
			fclose(fp);
			break;
		}
		V(semid, 0);
		fputs(buf[i], fp);
		printf("put runs %d times using the %d shm\n", d++, i);
		i = (i + 1) % 4;
	}
	V(semid, 2);
	printf("put finish\n");
	exit(0);
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
