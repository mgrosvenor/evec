#include <stdio.h>
#include <stdlib.h>

#define EV_FALL
#include "evec.h"

static int test1()
{
    int* a = evinit(int);
    if(evcnt(a) != 0) return 0;
    if(evvsz(a) != EV_INIT_COUNT) return 0;
    evfree(a);
    return 1;
}

static int test2()
{
    int* a = evinisz(128);
    if(evcnt(a) != 0) return 0;
    if(evvsz(a) != EV_INIT_COUNT) return 0;
    evfree(a);
    return 1;
}

static int test3()
{
    int* a = evini(128,64);
    if(evcnt(a) != 0) return 0;
    if(evvsz(a) != 64) return 0;
    evfree(a);
    return 1;
}

static int test4()
{
    int* a = evini(128,64);
    for(int i = 0; i < 1000; i++){
        evpsh(a,i);
    }

    for(int i = 0; i < 1000; i++){
        int* ai = evidx(a,i);
        if( *ai != i){ return 0;}
    }

    evfree(a);
    return 1;
}

static int test5()
{
    char** a = evini(128,64);
    for(int i = 0; i < 1000; i++){
        evpsh(a,"AA");
    }

    for(int i = 0; i < 1000; i++){
        char* ai = evidx(a,i);
        if( strcmp(ai,"AA"))return 0;
    }

    evfree(a);
    return 1;
}


static int test6()
{
    char** a = evini(128,0);
    for(int i = 0; i < 1000; i++){
        evpsh(a,"AA");
    }

    for(int i = 0; i < 1000; i++){
        char* ai = evidx(a,i);
        if( strcmp(ai,"AA"))return 0;
    }

    evfree(a);
    return 1;
}


static int test7()
{
    char** a = NULL;
    char* strs[] = {
            "Test",
            "Best",
            "Rest",
            "Vector",
            "Victor",
            0
    };

    for(int i = 0; strs[i]; i++){
        evpsh(a, strs[i]);
    }

    for(int i = 0; i < evcnt(a); i++){
        if(strcmp(a[i],strs[i])) return 0;
    }

    a = evfree(a);
    return 1;
}


static int compare(const void* lhs, const void* rhs)
{
    int* a = (int*)lhs;
    int* b = (int*)rhs;
    return *a - *b;
}

static int test8() {
    int* a = NULL;
    int ints[] = { 4,2,6,10,8 };
    int ints_srt[] = { 2,4,6,8,10 };
    const int ints_cnt = sizeof(ints)/sizeof(int);

#define REPEATS 1000
    for(int i = 0; i < REPEATS; i++){
        for(int j = 0; j < ints_cnt; j++){
            evpsh(a, ints[j]);
        }
    }

    if(evcnt(a) != REPEATS * ints_cnt) return 0;


    evsort(a,compare);

    //Remove duplicates
    for(int i = 1; i < evcnt(a); i++){
        if(a[i] == a[i-1]){
            evdel(a,i);
            i--;
        }
    }

    if(evcnt(a) != ints_cnt) {
        printf("%li != %i", evcnt(a) , ints_cnt);
        return 0;
    }

    for(int i =0; i < evcnt(a); i++){
        if(a[i] != ints_srt[i]) {
            printf("%i != %i", a[i], ints_srt[i]);
            return 0;
        }
    }

    a = evfree(a);
    return 1;
}

static int test9()
{
    int* a = evini(128,64);
    for(int i = 0; i < 1000; i++){
        evpsh(a,i);
    }

    for(int i = 0; i < 1000; i++){
        evdel(a,0);
    }

    if(evcnt(a) != 0) return 0;

    evfree(a);
    return 1;
}

static int test10()
{
    int* a = evini(sizeof(int), 0);
    for(int i = 0;i < 1000;i++){
        evpsh(a, i);
    }

    for(int i = 0;i < 1000;i++){
        evpop(a);
    }

    if(evcnt(a) != 0) return 0;

    evfree(a);
    return 1;
}

typedef int (*test_fn)();
typedef struct {
    char* name;
    test_fn run;
} Test;


Test tests[] = {
    {"evinit",      test1},
    {"evinisz",     test2},
    {"evini",       test3},
    {"evpsh",       test4},
    {"evpsh str",   test5},
    {"evpsh str0",  test6},
    {"evpsh str3",  test7},
    {"evpsh sort del",  test8},
    {"evpsh del",  test9},
    {"evpsh pop",  test10},
    {0}
};



int main(void)
{

    for(int i = 0; tests[i].name != 0; i++){
        printf("Running test %i: %s ...", i+1, tests[i].name);
        int result = tests[i].run();
        printf("%s\n", result ? "Success" : "Fail");
        if(!result){
            return -1;
        }
    }

    return 0;
}
