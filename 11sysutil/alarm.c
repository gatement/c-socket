#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>

void myhandler(int sig)
{
	printf("got signal: %d\n", sig);	
	alarm(2);
}

int main(void)
{
	signal(SIGALRM, myhandler);
	alarm(2);
	while(1)
	{
		sleep(1);
		//alarm(0);
	}
}
