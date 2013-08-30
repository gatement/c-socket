#include <unistd.h>
#include <stdio.h>

void myloop(int i, int (*func)(int))
{
    while(1)
    {
        sleep(2);
        i++;
        (*func)(i);
    }
}

void myprint()
{
    printf("%s:%d\n", "very good", 100);
}
