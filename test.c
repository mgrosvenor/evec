#include <stdio.h>
#include <stdlib.h>

#define EV_FALL
#include "evec.h"

/* Test 1
 * - Use the default initialization function evinit()
 * - Test that automatic allocation is sane ie, object count is 0, slot count is
 *   8 (or whatever EV_INIT_COUNT is set to).
 * - Test that evfree() works (with valgrind).
 * */
static int test1()
{
    int* a = evinit(int);
    if(evcnt(a) != 0) return 0;
    if(evvsz(a) != EV_INIT_COUNT) return 0;
    evfree(a);
    return 1;
}

/* Test 2
 * - Use the slot size based initialization function evinisz()
 * - Test that automatic allocation is sane ie, object count is 0, slot count is
 *   8 (or whatever EV_INIT_COUNT is set to).
 * - Test that evfree() works (with valgrind).
 * */
static int test2()
{
    int* a = evinisz(128);
    if(evcnt(a) != 0) return 0;
    if(evvsz(a) != EV_INIT_COUNT) return 0;
    evfree(a);
    return 1;
}

/* Test 3
 * - Use the slot size and count based initialization function evinisz()
 * - Test that automatic allocation is sane ie, object count is 0, slot count is
 *   as specifed (e.g. 64)
 * - Test that evfree() works (with valgrind).
 * */
static int test3()
{
    int* a = evini(128,64);
    if(evcnt(a) != 0) return 0;
    if(evvsz(a) != 64) return 0;
    evfree(a);
    return 1;
}

/* Test 4
 * - Use the slot size and count based initialization function evinisz()
 * - Slot sizes are set to 128B, which is much larger than an int.
 * - Push 1000 ints into vector.
 * - Test that automatic memory growing works (with valgrind).
 * - Test the evidx() function returns pointer to the right places.
 * - Test that evfree() works (with valgrind).
 */
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

/* Test 5
 * - Similar to test 4, but this time using string literals for fun.
 * - Use the slot size and count based initialization function evinisz()
 * - Slot sizes are set to 128B, which is much larger than an int.
 * - Push 1000 strings into vector.
 * - Test that automatic memory growing works (with valgrind).
 * - Test the evidx() function returns pointer to the right places.
 * - Test that evfree() works (with valgrind).
 */
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

/* Test 6
 * - Similar to test 5, but this time string pointers.
 * - Use the slot size and count based initialization function evinisz()
 * - Slot sizes are set to 128B, which is much larger than an int.
 * - Push 1000 strings into vector.
 * - Test that automatic memory growing works (with valgrind).
 * - Test the evidx() function returns pointer to the right places.
 * - Test that evfree() works (with valgrind).
 */
static int test6()
{
    char** a = NULL;
    char* str = "AA";
    for(int i = 0; i < 1000; i++){
        evpsh(a,str);
    }

    for(int i = 0; i < 1000; i++){
        char** ai = evidx(a,i);
        if( strcmp(*ai,str))return 0;
    }

    evfree(a);
    return 1;
}

/* Test 7
 * - Similar to test 5, but this time with an array of strings
 * - Use the slot size and count based initialization function evinisz()
 * - Slot sizes are set to 128B, which is much larger than an int.
 * - Push 1000 strings into vector.
 * - Test that automatic memory growing works (with valgrind).
 * - Test the evidx() function returns pointer to the right places.
 * - Test that evfree() works (with valgrind).
 */
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


/* Test 8
 * - Push 5000 ints into a vector, out of order. Sort them and remove duplicates.
 * - Test that automatic memory growing works (with valgrind).
 * - Test that evsort() works
 * - Test that evdel() function works
 * - Test that evfree() works (with valgrind).
 */
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

/* Test 9
 * - Use size and slot count initializer
 * - Push 1000 ints into a vector.
 * - Delete 1000 times from the front of the vector.
 * - Test that automatic memory growing works (with valgrind).
 * - Test that evidx() works
 * - Test that evdel() function works
 * - Test that evfree() works (with valgrind).
 */
static int test9()
{
    int* a = evini(128,64);
    for(int i = 0; i < 1000; i++){
        evpsh(a,i);
    }

    for(int i = 0; i < 999; i++){
        evdel(a,0);
        int* ai = evidx(a,0);
        if(*ai != i +1) return 0;
    }

    evdel(a,0);
    if(evcnt(a) != 0) return 0;

    evfree(a);
    return 1;
}


/* Test 10
 * - Use size and slot count initializer
 * - Test initialising with zero slots.
 * - Push 1000 ints into a vector.
 * - Pop 1000 times from the tail of the vector.
 * - Test that automatic memory growing works (with valgrind).
 * - Test that evidx() works
 * - Test that evpop() function works
 * - Test that evfree() works (with valgrind).
 */
static int test10()
{
    int* a = evini(sizeof(int), 0);
    for(int i = 0;i < 1000;i++){
        evpsh(a, i);
    }

    for(int i = 0;i < 999;i++){
        evpop(a);
        int* a0 = evidx(a,0);
        int* ac = evidx(a,evcnt(a) - 1);

        if(*a0 != 0) return 0;
        if(*ac != 999 - 1 - i) return 0;
    }

    evpop(a);
    if(evcnt(a) != 0) return 0;

    evfree(a);
    return 1;
}

/* Test 11
 * - Use the slot size based initialization function evinisz()
 * - Push 1000 ints into the vector.
 * - Test the memory growing works (with valgrind)
 * - Make a copy of the vector.
 * - Test that the copies are the same.
 * - Test that evfree() works (with valgrind).
 * */
static int test11()
{
    int* a = evinisz(128);
    int* b = NULL;

    for(int i = 0; i < 1000; i++){
        evpsh(a,i);
    }

    b = evcpy(a);

    for(int i = 0; i < 1000; i++){
        int* ai = (int*)evidx(a,i);
        int* bi = (int*)evidx(b,i);

        //Check that these are indeed different peices of memory
        if(ai == bi) return 0;

        //But that they have the same value
        if(*ai != *bi) return 0;
    }

    evfree(a);
    evfree(b);
    return 1;
}


typedef int (*test_fn)();
typedef struct {
    char* name;
    test_fn run;
} Test;


Test tests[] = {
    {"evinit",          test1},
    {"evinisz",         test2},
    {"evini",           test3},
    {"evpsh",           test4},
    {"evpsh str",       test5},
    {"evpsh str0",      test6},
    {"evpsh str3",      test7},
    {"evpsh sort del",  test8},
    {"evpsh del",       test9},
    {"evpsh pop",       test10},
    {"evpsh copy",      test11},
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
