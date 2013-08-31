#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "helper.h"

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while (0)

int main(void)
{
	int sock;
	// sock = socket(AF_INET, SOCK_STREAM, 0)
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		ERR_EXIT("socket error");
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* inet_aton("127.0.0.1", &servaddr.sin_addr); */

	if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		ERR_EXIT("connect error");
	}

	char buf[1024];
	int n, ret;
	memset(&buf, 0, sizeof(buf));

	while (fgets(buf, sizeof(buf), stdin) != NULL)
	{
		// fgets also captures the '\n'
		writen(sock, &buf, strlen(buf));

		// readline also return the '\n'
		ret = readline(sock, buf, sizeof(buf));
		if(ret == -1)
		{
			ERR_EXIT("connect error");
			break;
		}
		else if(ret == 0)
		{
			printf("server closed\n");
			break;
		}

		fputs(buf, stdout);

		memset(buf, 0, sizeof(buf));
	}

	close(sock);
	return 0;
}
