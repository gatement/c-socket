#include <unistd.h>

int main(void)
{
	pid_t pid1, pid2;
	pid1 = fork();
	pid2 = fork();
	while(1)
	{
		sleep(2);
	}

	return 0;
}
