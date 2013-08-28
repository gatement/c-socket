#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TCP_PORT 13000
#define SERVER "127.0.0.1"

// return 0: success, -1: error
int main(void)
{
    int result;
    int socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(-1 == socketFD)
    {
        printf("new socket error.\n");
        return -1;    
    }

    struct sockaddr_in stSockAddr;
    memset(&stSockAddr, 0, sizeof(stSockAddr));
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(TCP_PORT);
    result = inet_pton(AF_INET, SERVER, &stSockAddr.sin_addr);
    if(0 > result)
    {
        printf("inet_pton: invalid address family.\n");
        close(socketFD);
        return -1;
    }
    else if(0 == result)
    {
        printf("inet_pton: invalid address string.\n");
        close(socketFD);
        return -1;
    }

    if(-1 == connect(socketFD, (struct sockaddr *)&stSockAddr, sizeof(stSockAddr)))
    {
        printf("connect failed.\n");
        close(socketFD);
        return -1;
    }

    // read & write
    
    close(socketFD);
    return 0;
}
