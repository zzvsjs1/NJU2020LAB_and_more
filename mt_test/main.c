#include "thread.h"

int a = 5, b = 6;

void print(int i)
{
    atomic_fetch_add
    a++;   
}

int main()
{
    create(print);
    create(print);

    join();

    printf("%d\n", a);
    
    return 0;
}