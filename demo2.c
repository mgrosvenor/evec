/*
 * Demo 2:
 * A demo showing EV extended functions in use.
 */

#include <stdio.h>
#include "evec.h"

int main(int argc, char** argv)
{
    void* a = evinisz(128);
    evpsh(a, "Test");
    evpsh(a, "Best");
    evpsh(a, "Rest");

    for(int i =0; i < evcnt(a); i++){
        char* s = (char*)evidx(a,i);
        printf("%i: %s\n", i, s);
    }

    a = evfree(a);
    return 0;
}
