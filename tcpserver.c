#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define TCP_PORT 13000
#define TCP_QUEUE_LEN 10
#define BUFFER_SIZE 255
#define WELCOME_MSG "welcome."

// reutrn 0: success, -1: error
int main(void)
{
    char buf[BUFFER_SIZE];
    strcpy(buf, WELCOME_MSG);

    int listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenSock == -1)
    {
        printf("new socket error: %s(%d)\n", strerror(errno), errno);
        return -1;    
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(TCP_PORT);

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // reuse address
    int yes = 1;
    if(setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        printf("set socket option error: %s(%d)\n", strerror(errno), errno);
        close(listenSock);
        return -1;    
    }

    if(bind(listenSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        printf("bind socket error: %s(%d)\n", strerror(errno), errno);
        close(listenSock);
        return -1;    
    }

    if(listen(listenSock, TCP_QUEUE_LEN) == -1)
    {
        printf("listen socket error: %s(%d)\n", strerror(errno), errno);
        close(listenSock);
        return -1;    
    }

    while(1)
    {
        printf("start accepting tcp connection...\n");

        bzero(&clientAddr, sizeof(clientAddr));
        int clientSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if(clientSock == -1)
        {
            printf("accept socket error: %s(%d)\n", strerror(errno), errno);
            sleep(3);
            continue;
        }

        // the flowing code doesn't work
        //printf("start connection from client %s:%d\n", clientAddr, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("accepted connection from client %d:%d\n", clientAddr, clientAddr.sin_addr.s_addr, ntohs(clientAddr.sin_port));

        send(clientSock, buf, strlen(buf), 0);

        close(clientSock);
    }

    close(listenSock);
    return 0;
}
