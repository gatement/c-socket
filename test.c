#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(void)
{
    char buf[100];
    strcpy(buf, "Johnson");
    buf[7] = '\0';
    fputs(buf, stdout);
}
