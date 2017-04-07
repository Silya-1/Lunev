#include "helper.h"


int udpRecive(struct sockaddr_in* udpSockIn, socklen_t* udpInSockLen);
int tcp_serve(struct sockaddr_in * tcpSocketIn, int numOfThreads);
double calc(double begin, double end, int numOfThreads);


typedef  struct Data_t
{
    double start_point;
    double end_point;
    pthread_t pthreads_id;
    int cpu;
    char trash[36 + 6400];
} Data;


int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		perror("not enought arguments");
		return -1;
	}

	char *endtr = NULL;
	int numOfThreads = strtol(argv[1], &endtr, 10);
	socklen_t udpInSockLen = sizeof(struct sockaddr_in);
	struct sockaddr_in udpSockIn = {0};
	printf("Wait server\n");
	if(udpRecive(&udpSockIn, &udpInSockLen) != 0)
	{
		perror("no server");
		return -1;
	}

	char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(udpSockIn.sin_addr.s_addr), addr, INET_ADDRSTRLEN);
    printf("Received broadcast request from supervisor at %s:%d\n", addr, ntohs(udpSockIn.sin_port));


    printf("connecting  server \n");
    struct sockaddr_in tcpSocketIn = {0};
    tcpSocketIn.sin_addr.s_addr = udpSockIn.sin_addr.s_addr;
    tcpSocketIn.sin_port = htons(8001);
    tcpSocketIn.sin_family = AF_INET;

    if (tcp_serve(&tcpSocketIn, numOfThreads) != 0)
    {
        perror("can not make tcp serve");
        return -1;
    }
   
	return 0;
}


int tcp_serve(struct sockaddr_in * tcpSocketIn, int numOfThreads)
{
	int socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socketFd < 0)
	{
		perror("error in socket");
		return -1;
	}

	int optval = 1;
    int keepidle = 3;
    int keepintvl = 1;
    int keepcnt = 3;

    setsockopt(socketFd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
    setsockopt(socketFd, SOL_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
    setsockopt(socketFd, SOL_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
    setsockopt(socketFd, SOL_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));

    if (connect(socketFd, (struct sockaddr *)tcpSocketIn, sizeof(struct sockaddr_in)) != 0){
    	perror("can not connect");
        return -1;
    }

    arg_t arguments = {0};

    if (send(socketFd, &numOfThreads, sizeof(int), 0) != sizeof(int))
    {
        perror("can not send num of numOfThreads");
        return -1;
    }

    if (recv(socketFd, &arguments, sizeof(arg_t), 0) != sizeof(arg_t))
    {
        perror(" can not recive arguments");
        return -1;
    }

    printf(" begin  ==  %d\nend    == %d\n", arguments.begin, arguments.end);

    double res = (float)calc(arguments.begin, arguments.end, numOfThreads);


    if(send(socketFd, &res, sizeof(double), 0) != sizeof(double))
    {
    	perror("can not send result");
    	return -1;
    }

    return 0;
    
}

int udpRecive(struct sockaddr_in* udpSockIn, socklen_t* udpInSockLen)
{
	int socketFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(socketFd < 0)
	{
		perror("can not make socket");
		return -1;
	}

	int optval = 1;
	if(setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		perror("can not put flags");
		return -1;
	}

	udpSockIn->sin_addr.s_addr = htonl(INADDR_ANY);
    udpSockIn->sin_port = htons(8003);
    udpSockIn->sin_family = AF_INET;

    if (bind(socketFd, (struct sockaddr *)udpSockIn, sizeof(struct sockaddr_in)) != 0)
    {
        perror("error in bind");
        return -1;
    }

    udpSockIn->sin_addr.s_addr = htonl(INADDR_ANY);
    udpSockIn->sin_port = htons(0);
    udpSockIn->sin_family = AF_INET;

    char buffer[1] = "";
    if (recvfrom(socketFd, buffer, 1, 0, (struct sockaddr *)udpSockIn, udpInSockLen) < 0)
        return -1;

    close(socketFd);

    return 0;
}


void *thread_func(void *data)
{
	cpu_set_t cpu_set;
	CPU_ZERO(&cpu_set);
	CPU_SET((*(Data*)data).cpu , &cpu_set);
	pthread_setaffinity_np((*(Data*)data).pthreads_id, sizeof(cpu_set_t), &cpu_set);
       double start_point = (*(Data*)data).start_point;
        double end_point = (*(Data*)data).end_point;
        double counter = start_point;
        double dx = 0.0000001;
        double sum = 0;

        while(counter<end_point)
        {
        	sum += counter*counter*dx;
       		counter +=dx;
    	}
    (*(Data*)data).start_point = sum;
    return NULL;
}

int minimum (int a, int b)
{
	return a > b ? b : a;
}

double calc (double begin, double end, int numOfThreads) {
   
    int n, max;
    max = sysconf(_SC_NPROCESSORS_ONLN);
    Data points[max]  __attribute__((aligned(128)));
   // printf("%p   %p\n", &points[0], &points[1]);
    n = numOfThreads;
    n = minimum(n, max);
    printf("%d\n", max);
    double integral_sum = 0;

    double count = end - begin;
    for(int i = 0; i<max; i++)
    {		
    	if(i < n)
    	{
       	 	points[i].start_point = begin + (count/n)*i;
        	points[i].end_point = points[i].start_point + (count/n);
        	points[i].cpu = i % n;
        	pthread_create(&(points[i].pthreads_id), NULL, thread_func,(void *)&(points[i]));
        }else
        {
        	points[i].start_point = begin + (count/n)*0;
        	points[i].end_point = points[i].start_point + (count/n);
        	points[i].cpu = i % max;
        	pthread_create(&(points[i].pthreads_id), NULL, thread_func,(void *)&(points[i]));
        }	
    }
    for(int i = 0; i<max; i++)
    {
    	pthread_join(points[i].pthreads_id, NULL);
    	if(i < n)
        	integral_sum += points[i].start_point;
    }

    printf("%f\n",(float)integral_sum);
    //free(pthreads_ids);
    //free(points);

    return integral_sum;
}