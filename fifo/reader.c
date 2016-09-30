#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define bufSize 4


int main(int argc, char const *argv[])
{
	char *translator = "/tmp/translator";
	int fd, bytes;
	if(argc != 1)
	{
		perror("not enought arguments");
		return 1;
	}
 
 	errno = 0;
	if(mkfifo(translator, 0777) & (errno != EEXIST))
	{
		perror("cannnot make translator fifo");
		return 1;
	}

	if((fd = open(translator, O_RDONLY)) <= 0)
	{
		perror("cannnot open tranclator fifo");
		return 1;
	}

	char tranclatorName[bufSize];
	if((bytes = read(fd, tranclatorName, bufSize)) <= 0)
	{
		perror("cannot read more");
		return 1;
	}

	printf("%s\n", tranclatorName);

	errno = 0;
	if(mkfifo(tranclatorName, 0777) && (errno != EEXIST))
	{
		perror("cannot make transfer fifo");
		return 1;
	}

	int fifo_fd;
	if((fifo_fd = open(tranclatorName, O_RDONLY | O_NONBLOCK)) <= 0)
	{
		perror("cannnot open tranclator fifo");
		return 1;
	}	
	
	sleep(2);
	char buf[bufSize];

	int read_num = 1;
	while(read_num > 0)
	{
		read_num = read(fifo_fd, buf, bufSize);
		write(STDOUT_FILENO, buf, read_num);
	}

	close(fifo_fd);
	close(fd);
	return 0;
}