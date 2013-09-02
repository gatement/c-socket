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
#include<poll.h>
#include "helper.h"

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)


int main(void)
{
    int count = 0;
    signal(SIGPIPE, SIG_IGN);
    int listenfd;
    if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        //  listenfd = socket(AF_INET, SOCK_STREAM, 0)
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

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");

    if (listen(listenfd, SOMAXCONN) < 0) 
        ERR_EXIT("listen error");

    struct sockaddr_in peeraddr; 
    socklen_t peerlen = sizeof(peeraddr); 

    int conn; 
    int i;

    struct pollfd client[2048];
    int maxi = 0; // max pollfd index

    for (i = 0; i < 2048; i++)
        client[i].fd = -1;

    int nready;
    client[0].fd = listenfd;
    client[0].events = POLLIN;

    while (1)
    {
        nready = poll(client, maxi + 1, -1);
        if (nready == -1)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("poll error");
        }

        if (nready == 0)
            continue;

        if (client[0].revents & POLLIN)
        {

            conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen); 
            if (conn == -1)
                ERR_EXIT("accept error");

            for (i = 1; i < 2048; i++)
            {
                if (client[i].fd < 0)
                {
                    client[i].fd = conn;
                    if (i > maxi)
                        maxi = i;
                    break;
                }
            }

            if (i == 2048)
            {
                fprintf(stderr, "too many clients\n");
                exit(EXIT_FAILURE);
            }

            printf("count = %d\n", ++count);
            printf("recv connect ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr),
                   ntohs(peeraddr.sin_port));

            client[i].events = POLLIN;

            if (--nready <= 0)
                continue;
        }

        for (i = 1; i <= maxi; i++)
        {
            conn = client[i].fd;
            if (conn == -1)
                continue;
            if (client[i].revents & POLLIN)
            {

                char recvbuf[1024] = {0};
                int ret = readline(conn, recvbuf, 1024);
                if (ret == -1)
                    ERR_EXIT("readline error");
                else if (ret  == 0) 
                {
                    //printf("client  close \n");
                    client[i].fd = -1;
                    close(conn);
                }

                fputs(recvbuf, stdout);
                writen(conn, recvbuf, strlen(recvbuf));

                if (--nready <= 0)
                    break;
            }
        }

    }

    return 0;
}

