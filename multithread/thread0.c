#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *ShowMessage(void *arg);
int i=0;

int main(int argc, char **argv)
{
    char *MsgOne = "This is Message One(thread1).\n";
    char *MsgTwo = "Message Two(thread2).\n";
    pthread_t OnePthread;
    void *ThRes;//Thread result

    //pthread_creat
    if ((pthread_create(&OnePthread, NULL, ShowMessage, (void*)MsgOne)) < 0)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", MsgTwo);

    //pthread_join
    /*if ((pthread_join(OnePthread, &ThRes)) < 0)
    {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    printf("The result of thread2 is %s\n", ThRes);
	*/

	while(1)
	{
		if(i > 20) break;
		sleep(1);
		printf("thread1: %d\n", i++);
	}
    printf("END\n");

    exit(EXIT_SUCCESS);
}

void *ShowMessage(void *arg)
{
    printf("thread2 starts, param is:%s\n", (char *)arg);
	while(1)
	{
		if(i > 20) break;
		sleep(1);
		printf("thread2: %d\n", i++);
	}
    pthread_exit("Pthread_end_Okayn");
}
