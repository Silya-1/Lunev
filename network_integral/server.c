#include "helper.h"


typedef struct CONECTION
{
	struct sockaddr_in sockIn;
    char received;
    char sent;
    char accepted;
    int sockFd;
    int numOfThreads;
    arg_t arg;
} connect_t;

double makeWorkesWork(int tcpSocketFd, int workNum);
int udpBroadcast();

int main(int argc, char const *argv[])
{
	if(argc != 2){
		perror("not enough arguments");
		return -1;
	}

	char* endptr = NULL;
	int workNum = strtol(argv[1], &endptr, 10);

	int tcpSocketFd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if(tcpSocketFd == -1)
	{
		perror("can not crate broadcast");
		return -1;
	}
	int optval = 1;
	if(setsockopt(tcpSocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		perror("error in setsockopt");
		return -1;
	}

	struct sockaddr_in tcpSocketIn = {0};
	tcpSocketIn.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpSocketIn.sin_port = htons(8001);
    tcpSocketIn.sin_family = AF_INET;

    if(bind(tcpSocketFd, (struct sockaddr *)&tcpSocketIn, sizeof(struct sockaddr_in)) < 0)
    {
		perror("error in bind");
		return -1;
    }

    printf("%d\n", workNum);
    if(listen(tcpSocketFd, workNum) < 0)
    {
    	perror("error in listen");
		return -1;
    }

    printf("finding workes ...\n");
    fflush(stdout);
    if(udpBroadcast() != 0){
    	perror("No users");
    	return -1;
    }


    double res = makeWorkesWork(tcpSocketFd, workNum);
    printf("%lf\n", (float)res);
	return 0;
}


double makeWorkesWork(int tcpSocketFd, int workNum)
{
	fd_set write_fd, read_fd;
	FD_ZERO(&write_fd);
	FD_ZERO(&read_fd);
	FD_SET(tcpSocketFd, &read_fd);

	int optval = 1;
    int keepidle = 3;
    int keepintvl = 1;
    int keepcnt = 3;

    double result = 0;
    socklen_t tcpSockInLen = sizeof(struct sockaddr_in);

    int accepted = 0;
    double begin = 0;
    double end = 1200;

    int totalThreads = 0;
    int workersKnown = 0;
    int finishClients = 0;

    char strAddr[INET_ADDRSTRLEN];
    connect_t *connection = calloc(workNum, sizeof(connect_t));

    struct timeval timeWait;
	timeWait.tv_sec = 3;
	timeWait.tv_usec = 0;

	printf("%d\n", workNum);


    while(1)
    {

    	timeWait.tv_sec = 3;

    	if(select(FD_SETSIZE, &read_fd, &write_fd, NULL,&timeWait) == 0 && workNum != workersKnown)
    	{
    		if(udpBroadcast() != 0){
    			perror("No users");
    			return -1;
    		}	
    	}

    	
    	if(FD_ISSET(tcpSocketFd, &read_fd))
    	{
    		if(accepted == workNum)
    		{
    			printf("There is an extra port\n");
    			int sockFd = accept(tcpSocketFd, NULL, NULL);
    			close(sockFd);
    		}else
    		{
    			connection[accepted].sockFd = accept(tcpSocketFd, (struct sockaddr *)&connection[accepted].sockIn, &tcpSockInLen);
    			setsockopt(connection[accepted].sockFd, SOL_SOCKET, SOCK_NONBLOCK, &optval, sizeof(optval));
    			optval = 1;
    			if(connection[accepted].sockFd < 0)
    			{
    				printf("can not accep the connection \n");
    				return -1;
    			}

    			setsockopt(connection[accepted].sockFd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
    			setsockopt(connection[accepted].sockFd, SOL_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
    			setsockopt(connection[accepted].sockFd, SOL_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
    			setsockopt(connection[accepted].sockFd, SOL_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));

    			inet_ntop(AF_INET, &(connection[accepted].sockIn.sin_addr), strAddr, INET_ADDRSTRLEN);
                printf("Worker at %s:%d connected.\n", strAddr, ntohs(connection[accepted].sockIn.sin_port));
                
				connection[accepted].received = 0;
				connection[accepted].numOfThreads = 0;
				connection[accepted].accepted = 1;
				connection[accepted].sent     = 0;      
                if (++accepted == workNum)
                    close(tcpSocketFd);
    		}
    	}

    	int i = 0;
    	for(int i = 0; i < workNum; i++)
    	{
    		
    		if(FD_ISSET(connection[i].sockFd, &read_fd))
    		{
    			if(workersKnown < workNum)
    			{
    				if(recv(connection[i].sockFd, &connection[i].numOfThreads, sizeof(connection[i].numOfThreads), 0) != sizeof(connection[i].numOfThreads))
    				{
    					perror("can not recive num of threads");
    					return -1;
    				}
    				workersKnown++;
    				totalThreads += connection[i].numOfThreads;
    			}
    			else
    			{
    				double cliientRes = 0;
    				if(recv(connection[i].sockFd, &cliientRes, sizeof(double), 0) != sizeof(double))
    				{
    					perror("can not recive result");
    					return -1;
    				} 

    				//printf("%lf\n", cliientRes);
    				connection[i].received = 1;
    				printf("%f\n", (float)cliientRes);
    				result += cliientRes;
    				//printf("%lf\n", result);
    				finishClients++;
    				close(connection[i].sockFd);
    			}
    		}else if(FD_ISSET(connection[i].sockFd, &write_fd))
    		{
    			arg_t arguments = {0};
    			arguments.begin = begin;
    			arguments.end = begin + connection[i].numOfThreads * (end - connection[0].arg.begin) / totalThreads;
    			begin = arguments.end;
    			connection[i].arg = arguments;
    			if(send(connection[i].sockFd, &connection[i].arg, sizeof(arg_t), 0) != sizeof(arg_t))
    			{
    				perror("can not send");
    				return -1;
    			}
    			connection[i].sent = 1;
    		}
    	}

    	if(finishClients == workNum)
    		break;

    	FD_ZERO(&write_fd);
    	FD_ZERO(&read_fd);



    	for(i = 0; i < workNum; i++)
    	{
    		if(connection[i].accepted && !connection[i].sent &&(workNum == workersKnown))
    		{
    			FD_SET(connection[i].sockFd, &write_fd);
    		}

    		if(connection[i].accepted && !connection[i].received && (workersKnown <= workNum))
    		{
    			FD_SET(connection[i].sockFd, &read_fd);
    		}
    	}

    	if(accepted != workNum)
    	{
    		FD_SET(tcpSocketFd, &read_fd);
    	}
    }
    free(connection);
	return result;
}

int udpBroadcast()
{
	int sock_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock_fd < 0)
	{
		perror("error in sock_fd");
		return -1;
	}

	int optval = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0)
        return -1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        return -1;


    struct sockaddr_in udpSocketIn = {0};

    printf("%u\n", INADDR_BROADCAST);
    udpSocketIn.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    udpSocketIn.sin_port = htons(8003);
    udpSocketIn.sin_family = AF_INET;

    char buffer[1] = "";
    if (sendto(sock_fd, buffer, 1, 0, (struct sockaddr *)&udpSocketIn, sizeof(struct sockaddr_in)) <= 0)
        return -1;

    close(sock_fd);
    
    return 0;
     
}