#include <stdio.h>
#include "mysocket.h"

char *onMsgReceived(char *msg);
char *getOutputMsg();
int logMsg(char *msg);

int main(void)
{
    int port = 5678;
    startListening(port, onMsgReceived, getOutputMsg, logMsg);
}

char *onMsgReceived(char *msg)
{
    printf("received: %s", msg);
    return "ok";
}

char *getOutputMsg()
{
    return "welcome!";
}

int logMsg(char *msg)
{
    printf("log: %s\n", msg);
    return 0;
}

