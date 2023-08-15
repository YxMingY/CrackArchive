#include "include/tinycthread.h"
#include <stdio.h>
#include <string.h>

int task(void *arg)
{
    int *a = arg;
    printf("I am child %d %d\n",*a,thrd_current());
    return 37;
}

int main()
{
    thrd_t t;
    int arg = 36;
    thrd_create(&t,&task,&arg);
    int a[9] = {};
    memset(a,1,9);
    for (int i = 0; i < 5; ++i) {
        printf("%d\n",a[i]);
    }

    int res;
    thrd_join(t,&res);
    printf("%d\n",res);
    return 0;
}