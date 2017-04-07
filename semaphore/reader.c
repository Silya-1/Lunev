#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h> 
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define SH_SIZE 10000
#define BUF_SIZE 10000
#define SEM_NUM 3

#define SEM_OP(sembuf_num, num, op, flg)\
			soups[sembuf_num].sem_num = num;\
			soups[sembuf_num].sem_op = op;\
			soups[sembuf_num].sem_flg = flg;\

int main(int argc, char const *argv[])
{
	if(argc != 1)
	{
		perror("not enough arguments");
		return -1;
	}
	char buf[BUF_SIZE];
	struct sembuf soups[6];

	int sh_id;
	char* shmptr;

	if((sh_id = shmget(ftok("zz", SH_SIZE), SH_SIZE, 0777 | IPC_CREAT)) < 0)
	{
		perror("shmget error");
		return -1;
	}

	if((shmptr = shmat(sh_id, 0, 0)) == (void*) -1)
	{
		perror("shmat error");
		return -1;
	}

	int key = ftok("zz", SEM_NUM);
	int sem_id = semget(key, SEM_NUM, 0666 | IPC_CREAT);

	SEM_OP(0, 2, 0, IPC_NOWAIT);
	SEM_OP(1, 2, 2, SEM_UNDO);

	if(semop(sem_id, soups, 2) != 0)
	{
		return -1;
	}

	SEM_OP(0, 1, -3, 0);
	SEM_OP(1, 0, -1, 0);
	SEM_OP(2, 0 , 1, SEM_UNDO);
	SEM_OP(3, 1, 1, SEM_UNDO);
	SEM_OP(4, 1, 3, 0);
	semop(sem_id, soups, 5);

	int read_num = 1;
	
	while(read_num > 0)//1 - 4 0 -1 
	{
		SEM_OP(0, 0, -1, 0);
		SEM_OP(1, 0, 1, 0);
		semop(sem_id, soups, 2);
		if(semctl(sem_id, 1, GETVAL) == 1)
		{
			perror("writer is dead");
			return -1;
		}

		read_num = *((long *)shmptr);
		
		if(read_num > 0)
		{
			memcpy(buf, shmptr + sizeof(long), read_num);
			//memset(shmptr, 0, SH_SIZE);
			write(1, buf, read_num);
		}

		SEM_OP(0, 0, -1, 0);
		semop(sem_id, soups, 1);

		if(read_num <= 0)
			break;
	}
	
	return 0;
}


