#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/prctl.h>
#include <errno.h>



int pid;
int counter = 128;
int out = 0;

void add_one(int signo)
{
	out += counter;
	counter >>= 1;
	kill(pid, SIGUSR1);
}

void add_zero(int signo)
{
	counter >>= 1;
	kill(pid, SIGUSR1);
}

void action_child_death(int signo)
{
	exit(0);
}

void next_action(int signo)
{

}

int main(int argc, char const *argv[])
{
	/* code */
	if(argc != 2)
	{
		perror("not enough arguments");
		return -1;
	}

	sigset_t set;
	int par_pid = getpid();

	struct sigaction one = {};
	struct sigaction zero = {};
	struct sigaction child_death = {};

	sigfillset(&one.sa_mask);
	sigfillset(&zero.sa_mask);
	sigfillset(&child_death.sa_mask);

	one.sa_handler = add_one;
	zero.sa_handler = add_zero;
	child_death.sa_handler = action_child_death;


	sigaction(SIGUSR1, &one, NULL);
	sigaction(SIGUSR2, &zero, NULL);
	sigaction(SIGCHLD, &child_death, NULL);

	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);

	
	sigprocmask(SIG_BLOCK, &set, NULL);
	sigemptyset(&set);

	
	pid = fork();


	
	if(pid == 0)
	{
		prctl(PR_SET_PDEATHSIG, SIGKILL);
		if(getppid() != par_pid)
			return -1;

		sigemptyset(&set);

		struct sigaction empty = {};
		sigfillset(&empty.sa_mask);
		empty.sa_handler = next_action;
		sigaction(SIGUSR1, &empty, NULL);

		int fd = open(argv[1], O_RDONLY);
		int read_num = 1;
		char symbol = 0;

		while(read_num > 0)
		{
			read_num = read(fd, &symbol, 1);
			if(read_num == 0)
				break;
		
			for(int i = 128; i >= 1; i >>= 1)
			{
				if(i & symbol)
					kill(par_pid, SIGUSR1);
				else
					kill(par_pid, SIGUSR2);
				sigsuspend(&set);
			}
			symbol = 0;

		} 

		exit(0);

	}
	else
	{
		while(1)
		{

			sigsuspend(&set);
			if(counter == 0)
			{
				write(1, &out, 1);
				fflush(stdout);
				counter = 128;
				out = 0;
			}

			
		}
		
	}
	return 0;
} 