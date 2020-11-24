#include <stdio.h>
#include <stdlib.h>

#define EV_FALL
#include "evec.h"

void hexdump(const void *data, int size)
{
    /* dumps size bytes of *data to stdout. Looks like:
     * [0000] 75 6E 6B 6E 6F 77 6E 20 30 FF 00 00 00 00 39 00 unknown 0.....9.
     */

    const unsigned char *p = data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[ 16*3 + 5] = {0};
    char charstr[16*1 + 5] = {0};
    for(n=1;n<=size;n++) {
        if (n%16 == 1) {
            /* store address for this line */
            snprintf(addrstr, sizeof(addrstr), "%.4lx", (size_t)(p-(unsigned char*)data));
        }

        c = *p;
        if (isalnum(c) == 0) {
            c = '.';
        }

        /* store hex str (for left side) */
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

        /* store char str (for right side) */
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

        if(n%16 == 0) {
            /* line completed */
            fprintf(stderr,"[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        } else if(n%8 == 0) {
            /* half line: add whitespaces */
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++; /* next byte */
    }

    if (strlen(hexstr) > 0) {
        /* print rest of buffer if not empty */
        fprintf(stderr,"[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}


void printi(uint64_t* a)
{
    for(int i =0; i < evcnt(a); i++){
        printf("%02llu ", a[i]);
    }

    printf("\n");
}

void prints(void* a)
{
    for(int i =0; i < evcnt(a); i++){
        printf("%s ", evidx(a,i));
    }

    printf("\n");
}

static int compare_uint64(const void *pa, const void *pb)
{
    uint64_t a = *(const uint64_t *)pa;
    uint64_t b = *(const uint64_t *)pb;
    return (a > b) ? -1 : (a < b) ? +1 : 0;
}


static int compare_str(const void *pa, const void *pb)
{
    const char* a = (const char*)pa;
    const char* b = (const char *)pb;
    return -strcmp(a,b);
}

int main(void)
{

    uint64_t* a = NULL;
    void* b = evinisz(128);

    for(uint64_t i = 0;i < 32;i++){
        evpsh(a, i);
        printi(a);
    }

//    for(int i = 0;i < 32;i++){
//        evpop(a);
//        printi(a);
//    }
//
//    for(int i = 0;i < 32;i++){
//        evpsh(a, i);
//        printi(a);
//    }
//
//    for(int i = 0;i < 32;i++){
//        evdel(a, 0);
//        printi(a);
//    }
//

    char val[3] = "AA";
    for(int i = 0;i < 32;i++){
        //val[0] += i;
        evpush(b, val, sizeof(val) );
        prints(b);
    }

//
//    for(int i = 0;i < 32;i++){
//        evdel(b, 0);
//        prints(b);
//    }


    evsort(a,compare_uint64);
    printi(a);

    evsort(b,compare_str);
    prints(b);


    return 0;
}
