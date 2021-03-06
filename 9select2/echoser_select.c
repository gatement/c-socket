#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<signal.h>
#include<sys/wait.h>
#include "helper.h"

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)


int main(void)
{
    
    signal(SIGPIPE, SIG_IGN);
    int listenfd;
	//  listenfd = socket(AF_INET, SOCK_STREAM, 0)  
    if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket error");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    /* servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
    /* inet_aton("127.0.0.1", &servaddr.sin_addr); */
    
    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt error");

    if (bind(listenfd, (struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");

    if (listen(listenfd, SOMAXCONN) < 0)
        ERR_EXIT("listen error");
    
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);
    
    int conn;
    int i;

    int client[FD_SETSIZE];
    int maxi = 0; // client array max index
    for (i = 0; i < FD_SETSIZE; i++)
	{
        client[i] = -1;
	}

    int nready;
    int maxfd = listenfd;
    fd_set rset;
    fd_set allset;
    FD_ZERO(&rset);
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

	printf("max conn: %d\n", FD_SETSIZE);

    while (1) 
	{
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready == -1) 
		{
            if (errno == EINTR)
			{
                continue;
			}
            ERR_EXIT("select error");
        }

        if (nready == 0)
            continue;

		printf("nready: %d\n", nready);

		// handle conn request
        if (FD_ISSET(listenfd, &rset)) 
		{
            conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
            if (conn == -1)
			{
                ERR_EXIT("accept error");
			}
            
			// store the new conn
            for (i = 0; i < FD_SETSIZE; i++)
			{
                if (client[i] < 0) 
				{
                    client[i] = conn;
                    if (i > maxi)
					{
                        maxi = i;
					}
                    break;
                } 
            }
            
            if (i == FD_SETSIZE) 
			{
                fprintf(stderr, "too many clients(max %d)\n", FD_SETSIZE);
                exit(EXIT_FAILURE);
            }

            printf("recv connect(%d) ip=%s port=%d\n", i, inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

            FD_SET(conn, &allset);
            if (conn > maxfd)
			{
                maxfd = conn;
			}

			// only one event or zero event, continue (becase I have handled it)
            if (--nready <= 0)
			{
                continue;
			}
        }

		// handle socket input (arrival)
        for (i = 0; i <= maxi; i++) 
		{
            conn = client[i];
            if (conn == -1)
			{
				// no conn in this index
                continue;
			}

            if (FD_ISSET(conn, &rset)) 
			{
                char recvbuf[1024] = {0};
                int ret = readline(conn, recvbuf, 1024);
                if (ret == -1)
				{
                    ERR_EXIT("readline error");
				}
                else if (ret  == 0) 
				{
                    printf("client close(%d)\n", i);
                    FD_CLR(conn, &allset);
                    client[i] = -1;
                    close(conn);
                }
        
                fputs(recvbuf, stdout);

				// echo back
                writen(conn, recvbuf, strlen(recvbuf));
                
				// only one arrival event, so can break the for loop
                if (--nready <= 0)
				{
                    break; 
				}
            }
        }
    }
        
    return 0;
}
