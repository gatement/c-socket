#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define UDP_PORT 13001
#define BUFFER_SIZE 255

// reutrn 0: success, -1: error
int main(void)
{
    ssize_t recvSize;
    char buf[BUFFER_SIZE];

    int listenSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(listenSock == -1)
    {
        printf("new socket error.\n");
        return -1;    
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(UDP_PORT);

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // reuse address
    int yes = 1;
    if(setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        printf("set socket option error.\n");
        close(listenSock);
        return -1;    
    }

    if(bind(listenSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        printf("bind socket error.\n");
        close(listenSock);
        return -1;    
    }

    while(1)
    {
        printf("start listening udp datagrams...\n");

        bzero(&clientAddr, sizeof(clientAddr));
        
        recvSize = recvfrom(listenSock, (void *) buf, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrLen); 
        if(recvSize < 0)
        {
            printf("recvfrom error.\n");
            sleep(3);
            continue;
        }
        else if(recvSize > 0)
        {
            buf[recvSize] = '\0';
            printf("received data from client [%d:%d]: %s\n", clientAddr, clientAddr.sin_addr.s_addr, ntohs(clientAddr.sin_port), buf);
        }
    }

    close(listenSock);
    return 0;
}
