#include "thread.h"

void print(int i)
{
    while (1)
    {
        printf("Hello %d\n", i);
    }
}

int main()
{
    create(print);
    create(print);
    
    return 0;
}