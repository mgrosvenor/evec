/*
 * Demo 1:
 * Simple demo showing EV core functions in use.
 */

#include <stdio.h>
#include "evec.h"

int main(int argc, char** argv)
{
    int* a = NULL;
    evpsh(a, 2);
    evpsh(a, 4);
    evpsh(a, 6);

    for(int i =0; i < evcnt(a); i++){
        printf("%i: %i\n", i, a[i]);
    }

    a = evfree(a);
    return 0;
}
