#include <stdio.h>
#include "sock.h"

int myfunc(int);

int main(void)
{
	printf("hello\n");
    myprint();

    int a = 100;
   // myloop(a, myfunc);
}

int myfunc(int a)
{
    printf("lgh: %d\n", a);
}

