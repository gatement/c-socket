#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define UDP_PORT 13001
#define SERVER "127.0.0.1"
#define BUFFER_SIZE 255
#define HELLO_MSG "hello, udp server."

// reutrn 0: success, -1: error
int main(void)
{
    int result;
    ssize_t recvSize;
    char buf[BUFFER_SIZE];
    strcpy(buf, HELLO_MSG);

    int clientSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(clientSock == -1)
    {
        printf("new socket error: %s(%d)\n", strerror(errno), errno);
        return -1;    
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
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

    int bytesSent = sendto(clientSock, buf, strlen(buf), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(bytesSent <= 0)
    {
        printf("send data error: %s(%d)\n", strerror(errno), errno);
        close(clientSock);
        return -1;
    }

    close(clientSock);
    return 0;
}
