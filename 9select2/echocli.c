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

	// print local addr
	struct sockaddr_in localaddr;
	socklen_t localaddrlen = sizeof(localaddr);
	if (getsockname(sock, (struct sockaddr *) &localaddr, &localaddrlen) < 0)
	{
		ERR_EXIT("getsockname error");
	}
	printf("local ip=%s, port=%d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));

	fd_set rset;
	FD_ZERO(&rset);
	
	int nready;
	int ret;
	int maxfd;
	int fd_stdin = fileno(stdin);
	if(fd_stdin > sock)
	{
		maxfd = fd_stdin;
	}
	else
	{
		maxfd = sock;
	}

	char buf[1024];
	while(1)
	{
		memset(buf, 0, sizeof(buf));

		FD_ZERO(&rset);
		FD_SET(fd_stdin, &rset);
		FD_SET(sock, &rset);

		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(nready == -1)
		{
			ERR_EXIT("select error");
			continue;
		}
		else if(nready == 0)
		{
			continue;
		}
		else
		{
			// echeck any socket input (arrival)
			if(FD_ISSET(sock, &rset))
			{
				ret = readline(sock, buf, sizeof(buf));
				if(ret == -1)
				{
					ERR_EXIT("connect error");
					continue;
				}
				else if(ret == 0)
				{
					printf("server closed\n");
					break;
				}

				fputs(buf, stdout);
			}

			// check user input to send 
			if(FD_ISSET(fd_stdin, &rset))
			{
				if (fgets(buf, sizeof(buf), stdin) == NULL)
				{
					continue;
				}
				
				// fgets also captures the '\n'
				writen(sock, &buf, strlen(buf));
			}
		}
	}

	close(sock);
	return 0;
}
