#include <stdio.h>
#include "sock.h"

int myfunc(int);

int main(void)
{
    int a = 100;
    init(a, myfunc);
}

int myfunc(int a)
{
    printf("lgh: %d\n", a);
}

