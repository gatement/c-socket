#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//void *workingthread(void *arg);

int i;
int (*recv) (int);

int init(int i0, int (*recv0)(int))
{
	i = i0;
	printf("got: %d\n", i);
	recv = recv0;
/*
	pthread_t workingthreadfd;
	if(pthread_create(&workingthreadfd, NULL, workingthread, NULL) < 0)
	{
		perror("pthread-create");
	}*/

	(*recv)(++i);
	
	return 0;
}

/*void *workingthread(void *arg)
{
    while(1)
    {
        sleep(2);
        i++;
		(*recv)(i);
    }
}*/

void send(int a)
{
    printf("%s: %d\n", "got", a);
}

