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
#define SERVER "127.0.0.1"
#define BUFFER_SIZE 255

// return 0: success, -1: error
int main(void)
{
    int result;
    int clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(clientSock == -1)
    {
        printf("new socket error: %s(%d)\n", strerror(errno), errno);
        return -1;    
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    result = inet_pton(AF_INET, SERVER, &serverAddr.sin_addr);
    if(result < 0)
    {
        printf("inet_pton: invalid address family: %s(%d)\n", strerror(errno), errno);
        close(clientSock);
        return -1;
    }
    else if(result == 0)
    {
        printf("inet_pton: invalid address string: %s(%d)\n", strerror(errno), errno);
        close(clientSock);
        return -1;
    }

    if(connect(clientSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        printf("connect failed: %s(%d)\n", strerror(errno), errno);
        close(clientSock);
        return -1;
    }

    char buf[BUFFER_SIZE]; 
    int recvlen = recv(clientSock, buf, BUFFER_SIZE, 0);
    if(recvlen > 0)
    {
        buf[recvlen] = '\0';
        printf("received: %s\n", buf);
    }

    close(clientSock);
    return 0;
}
