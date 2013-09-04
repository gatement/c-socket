#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *Thread1(void *arg);
void *ShowMessage(void *arg);

int main(int argc, char **argv)
{
    char *MsgOne = "This is Message One(thread0).\n";
    pthread_t OnePthread;

    //pthread_create
    if (pthread_create(&OnePthread, NULL, Thread1, NULL) < 0)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", MsgOne);

	int i=0;
	while(1)
	{
		if(i > 20) break;
		i++;
		sleep(1);
		//printf("thread1: %d\n", i++);
	}
    printf("thread 0 END\n");

    exit(EXIT_SUCCESS);
}

void *Thread1(void *arg)
{ 
	char * MsgTwo = "from thread1";
	pthread_t TwoPthread;
    //pthread_creat
    if ((pthread_create(&TwoPthread, NULL, ShowMessage, (void*)MsgTwo)) < 0)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    printf("thread 1 END\n");
    pthread_exit("thread 1 end");
}

void *ShowMessage(void *arg)
{
	int i=0;
    printf("thread2 starts, param is:%s\n", (char *)arg);
	while(1)
	{
		if(i > 20) break;
		sleep(1);
		printf("thread2: %d\n", i++);
	}
    printf("thread 2 END\n");
}
 
