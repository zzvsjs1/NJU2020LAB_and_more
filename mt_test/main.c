#include "thread.h"

void print(int i)
{
    while (true)
    {
        printf("Hello thread %d!\n", i);
    }
}

int main()
{
    create(print);
    create(print);
    
    return 0;
}