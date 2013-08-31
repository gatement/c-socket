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
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        //  listenfd = socket(AF_INET, SOCK_STREAM, 0)
        ERR_EXIT("socket error");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* inet_aton("127.0.0.1", &servaddr.sin_addr); */

    if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("connect error");

	struct packet sendrecvbuf;	
	int n, ret;
	memset(&sendrecvbuf, 0, sizeof(sendrecvbuf));

    while (fgets(sendrecvbuf.buf, sizeof(sendrecvbuf.buf), stdin) != NULL)
    {
		n = strlen(sendrecvbuf.buf);
		sendrecvbuf.len = htonl(n);
        writen(sock, &sendrecvbuf, 4 + n);

		n = 4;
        ret = readn(sock, &sendrecvbuf.len, n);
		if(ret == -1)
		{
			ERR_EXIT("connect error");
			break;
		}
		else if(ret < n)
		{
			printf("server closed\n");
			break;
		}

		n = ntohl(sendrecvbuf.len);
        ret = readn(sock, sendrecvbuf.buf, n);
		if(ret == -1)
		{
			ERR_EXIT("connect error");
			break;
		}
		else if(ret < n)
		{
			printf("server closed\n");
			break;
		}

        fputs(sendrecvbuf.buf, stdout);
		memset(&sendrecvbuf, 0, sizeof(sendrecvbuf));
    }

    close(sock);
    return 0;
}
