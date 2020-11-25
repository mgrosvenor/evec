
/*
 * Demo 3:
 * A demo showing EV advanced functions in use.
 */

#include <stdio.h>

#define EV_FDEL
#define EV_FSORT
#define EV_INIT_COUNT 5
#include "evec.h"

int compare(const void* lhs, const void* rhs)
{
    int* a = (int*)lhs;
    int* b = (int*)rhs;

    return *b - *a;
}

int main(int argc, char** argv)
{
    int* a = NULL;

    printf("Initial Set\n");
    evpsh(a, 6);
    evpsh(a, 4);
    evpsh(a, 2);
    evpsh(a, 2);
    evpsh(a, 4);
    evpsh(a, 6);

    for(int i = 0; i < evcnt(a); i++){
        printf("%i: %i\n", i, a[i]);
    }
    printf("\n");

    printf("Sorted Set\n");
    evsort(a,compare);

    for(int i = 0; i < evcnt(a); i++){
        printf("%i: %i\n", i, a[i]);
    }
    printf("\n");


    printf("De-duplicated Set\n");
    for(int i = 1; i < evcnt(a); i++){
        if(a[i] == a[i-1]){
            evdel(a,i);
            i--;
        }
    }

    for(int i = 0; i < evcnt(a); i++){
        printf("%i: %i\n", i, a[i]);
    }
    printf("\n");

    a = evfree(a);
    return 0;
}
