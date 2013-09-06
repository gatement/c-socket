#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *Thread1(void *arg)
{
	int i = 0;
	printf("thread1 pid: %d\n", pthread_self());
	while(1)
	{
		sleep(2);
		//printf("thread1: %d\n", i++);
	}
	printf("thread1 end.\n");
}

int main(void)
{
	printf("thread0 pid: %d\n", pthread_self());

	pthread_t thread1;
	if (pthread_create(&thread1, NULL, Thread1, NULL) < 0)
	{
		perror("pthread_create error");
		exit(EXIT_FAILURE);
	}

	printf("thread0: thread1 pid: %d\n", thread1);
	int i = 1000000;
	while(1)
	{
		sleep(2);
		//printf("thread0: %d\n", i--);
	}

	void *thread1Res;
	if (pthread_join(thread1, &thread1Res) < 0)
	{
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	printf("thread0 end.\n");

	return 0;
}
