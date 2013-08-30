#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(void)
{
    char buf[100] = {0};
	while(fgets(buf, sizeof(buf), stdin) != NULL)
	{
		fputs(buf, stdout);
	}
}
