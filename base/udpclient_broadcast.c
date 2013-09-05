#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define UDP_PORT 5678
#define SERVER "127.0.0.1"
#define BUFFER_SIZE 255

// reutrn 0: success, -1: error
int main(void)
{
    int recvSize;
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);
    strcpy(buf, "hello, udp server.");

    int clientSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(clientSock == -1)
    {
        printf("new socket error: %s(%d)\n", strerror(errno), errno);
        return -1;    
    }
  
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "eth0");
    int broadcastAddr = 0;
    if(ioctl(clientSock, SIOCGIFBRDADDR, &ifr) != -1)
    {
        struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_broadaddr;
        memset(buf, 0, BUFFER_SIZE);
        strcpy(buf, inet_ntoa(sin->sin_addr));
        printf("Broadcast is %s\n", buf);

        broadcastAddr = sin->sin_addr.s_addr;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    //serverAddr.sin_addr.s_addr = inet_addr(SERVER);
    serverAddr.sin_addr.s_addr = broadcastAddr;
    //serverAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    memset(buf, 0, BUFFER_SIZE);
    strcpy(buf, inet_ntoa(serverAddr.sin_addr));
    printf("tartet ip is %s\n", buf);

    int broadcastEnable=1;
    int ret=setsockopt(clientSock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

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
