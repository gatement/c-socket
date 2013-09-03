#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "helper.h"
#include "sysutil.h"


int main(void)
{
    int count = 0;
    int listenfd;
    if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt");

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");

    if (listen(listenfd, SOMAXCONN) < 0)
        ERR_EXIT("listen");

    int epollfd = epoll_create1(EPOLL_CLOEXEC);

    struct epoll_event event;
    event.data.fd = listenfd;
    //event.events = EPOLLIN | EPOLLET; //EPOLLET = EPOLL Edge Trigger
    event.events = EPOLLIN; //EPOLLLT = EPOLL Level Trigger
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);

	int maxevents = 2048;
    struct epoll_event events[maxevents];
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int conn;
    int i;

    int nready;
    while (1)
    {
        nready = epoll_wait(epollfd, events, maxevents, -1);
        if (nready == -1)
        {
            if (errno == EINTR)
                continue;

            ERR_EXIT("epoll_wait");
        }
        if (nready == 0)
            continue;

        if ((size_t)nready == maxevents)
            ERR_EXIT("too many events");

        for (i = 0; i < nready; i++)
        {
            if (events[i].data.fd == listenfd)
            {
                peerlen = sizeof(peeraddr);
                conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen);
                if (conn == -1)
                    ERR_EXIT("accept");

                printf("%d: ip=%s port=%d\n", ++count, inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

                activate_nonblock(conn);

                event.data.fd = conn;
                //event.events = EPOLLIN | EPOLLET;
                event.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, conn, &event);
            }
            else if (events[i].events & EPOLLIN)
            {
                conn = events[i].data.fd;
                if (conn < 0)
                    continue;

                char recvbuf[1024] = {0};
                int ret = readline(conn, recvbuf, 1024);
                if (ret == -1)
                    ERR_EXIT("readline");
                if (ret == 0)
                {
                    //printf("client close\n");
                    close(conn);

                    event = events[i];
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, conn, &event);
                }

                fputs(recvbuf, stdout);
                writen(conn, recvbuf, strlen(recvbuf));
            }
        }
    }

    return 0;
}
