#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TCP_PORT 13000
#define TCP_QUEUE_LEN 10
#define BUFFER_SIZE 255
#define WELCOME_MSG "welcome."

// reutrn 0: success, -1: error
int main(void)
{
    int listenSockFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(-1 == listenSockFD)
    {
        printf("new socket error.\n");
        return -1;    
    }

    struct sockaddr_in listenSockAddr;
    memset(&listenSockAddr, 0, sizeof(listenSockAddr));
    listenSockAddr.sin_family = AF_INET;
    listenSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    listenSockAddr.sin_port = htons(TCP_PORT);

    if(-1 == bind(listenSockFD, (struct sockaddr *)&listenSockAddr, sizeof(listenSockAddr)))
    {
        printf("bind socket error.\n");
        close(listenSockFD);
        return -1;    
    }

    if(-1 == listen(listenSockFD, TCP_QUEUE_LEN))
    {
        printf("listen socket error.\n");
        close(listenSockFD);
        return -1;    
    }

    for(;;)
    {
        printf("start accept connection...\n");

        struct sockaddr_in clientSockAddr;
        bzero(&clientSockAddr, sizeof(clientSockAddr));
        socklen_t clientSockAddrLen = sizeof(clientSockAddr);
        int clientSockFD = accept(listenSockFD, (struct sockaddr *)&clientSockAddr, &clientSockAddrLen);
        if(-1 == clientSockFD)
        {
            printf("accept socket error.\n");
            return -1;    
        }
        printf("start connection from client %s:%d\n", clientSockAddr, inet_ntoa(clientSockAddr.sin_addr), ntohs(clientSockAddr.sin_port));

        char buf[BUFFER_SIZE];
        strcpy(buf, WELCOME_MSG);
        send(clientSockFD, buf, strlen(buf), 0);

        close(clientSockFD);
    }

    close(listenSockFD);
    return -1;
}
