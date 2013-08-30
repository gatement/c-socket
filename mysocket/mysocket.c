#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define TCP_QUEUE_LEN 10
#define BUFFER_SIZE 255

int startListening(int port, char* (*onMsgReceived)(char*), char* (*getOutputMsg)(), int (*logMsg)(char*))
{
    char buf[BUFFER_SIZE];

    int listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenSock == -1)
    {
        bzero(buf, BUFFER_SIZE);
        sprintf(buf, "new socket error: %s(%d)", strerror(errno), errno);
        (*logMsg)(buf);
        return -1;    
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(port);

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // reuse address
    int yes = 1;
    if(setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        bzero(buf, BUFFER_SIZE);
        sprintf(buf, "set socket option error: %s(%d)", strerror(errno), errno);
        (*logMsg)(buf);
        close(listenSock);
        return -1;    
    }

    if(bind(listenSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        bzero(buf, BUFFER_SIZE);
        sprintf(buf, "bind socket error: %s(%d)", strerror(errno), errno);
        (*logMsg)(buf);
        close(listenSock);
        return -1;    
    }

    if(listen(listenSock, TCP_QUEUE_LEN) == -1)
    {
        bzero(buf, BUFFER_SIZE);
        sprintf(buf, "listen socket error: %s(%d)", strerror(errno), errno);
        (*logMsg)(buf);
        close(listenSock);
        return -1;
    }
    
    bzero(buf, BUFFER_SIZE);
    sprintf(buf, "start accepting tcp connection...");
    (*logMsg)(buf);

    bzero(&clientAddr, sizeof(clientAddr));
    int clientSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if(clientSock == -1)
    {
        bzero(buf, BUFFER_SIZE);
        sprintf(buf, "accept socket error: %s(%d)", strerror(errno), errno);
        (*logMsg)(buf);
        return -1;
    }

    bzero(buf, BUFFER_SIZE);
    sprintf(buf, "accepted connection from client %s:%d", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    (*logMsg)(buf);
    
    // send
    char *msgToSend = (*getOutputMsg)(); 
    if(strlen(msgToSend) > 0)
    {
        send(clientSock, msgToSend, strlen(msgToSend), 0);
    }

    while(1)
    {
        // receive
        int recvlen = recv(clientSock, buf, BUFFER_SIZE, 0);
        if(recvlen > 0)
        {
            buf[recvlen] = '\0';
            char *msgToSend = (*onMsgReceived)(buf);
            if(strlen(msgToSend) > 0)
            {
                send(clientSock, msgToSend, strlen(msgToSend), 0);
            }
        }
    }

    close(clientSock);
    close(listenSock);
    return 0;
}

