#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

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

	fcntl(listenSock, F_SETFL, O_NONBLOCK);
	
	// hold peer sock FDs
	int peers[TCP_QUEUE_LEN] = {0};
	int i, peerLen = 0;
	printf("start accepting tcp connection...\n");

    while(1)
    {
		//printf("loop\n");
		
		// check to see if there is any connection request
        bzero(&clientAddr, sizeof(clientAddr));
        int clientSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if(clientSock > 0)
        {
			// add peer to the peer list
			for(i=0; i<TCP_QUEUE_LEN; i++)
			{
				if(peers[i] == 0)
				{
					peers[i] = clientSock;
					break;
				}
			}

			if(i == TCP_QUEUE_LEN)
			{
				// max conn exceeded
				printf("too many connections(max %d).\n", TCP_QUEUE_LEN);
				close(clientSock);
			}
			else
			{
				peerLen = peerLen < i+1 ? i+1 : peerLen;

				fcntl(clientSock, F_SETFL, O_NONBLOCK);
				printf("accepted connection(%d) %s:%d\n", i, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
				
				// send welcome msg
				memset(buf, 0, BUFFER_SIZE);
				strcpy(buf, WELCOME_MSG);
				send(clientSock, buf, strlen(buf), 0);
			}
        }

		// check if any data arrived from any peer
		for(i=0; i < peerLen; i++)
		{
			if(peers[i] != 0)
			{
				memset(buf, 0, BUFFER_SIZE);
				int recvlen = recv(peers[i], buf, BUFFER_SIZE, 0);
				if(recvlen == 0)
				{
					printf("peer %d closed\n", i);
					close(peers[i]);
					peers[i] = 0;
				}
				if(recvlen > 0)
				{
					buf[recvlen] = '\0';
					printf("received(%d): %s", i, buf);

					memset(buf, 0, BUFFER_SIZE);
					strcpy(buf, "ok");
					send(peers[i], buf, strlen(buf), 0);
				}
			}
		}
		
		//sleep(1);
    }

	for(i=0; i < peerLen; i++)
	{
		close(peers[i]);
	}
    close(listenSock);
    return 0;
}
