/*
 * Easy Vector (EV)
 * ================
 * This is an easy to use vector type for C programs where simplicity is the
 * key concern. It's designed for situations like options parsing where building
 * up an unknown amount of state is required, but it is likely to be small.
 *
 * The neat thing about this vector is that it maintains array semantics by
 * hiding state near the allocated memory. Some (optional) macro magic makes it
 * even easier to use.
 *
 * Documentation
 * =============
 * Please see README.md
 *
 * Legal Stuff
 * ============
 * Copyright (c) 2020, Matthew P. Grosvenor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EVH_
#define EVH_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h>
#include <ctype.h>
#include <libgen.h>
#include <unistd.h>

/*
 * Build Time Parameters
 * ===========================================================================
 */


#ifndef EV_HARD_EXIT
#define EV_HARD_EXIT       1 //Fail to exit(0xDEAD).
#endif

#ifndef EV_INIT_COUNT
#define EV_INIT_COUNT      8 //Start with 8 slots
#endif

#ifndef EV_GROWTH_FACTOR
#define EV_GROWTH_FACTOR   2 //Grow by a factor of x when space runs out
#endif

#ifndef EV_PEDANTIC
#define EV_PEDANTIC 1 //If this is set, pedantic error checking is performed
#endif

#ifndef EV_DEBUG
#define EV_DEBUG 0 //If this is set, debug printing is enabled
#endif

#define EV_MAJOR 1
#define EV_MINOR 3
#define EV_RELEASE 0 //If release is 1, this is an offical release version

/*
 * Forward declarations of the the EV interface functions
 * ===========================================================================
 */

/**
 * Easy allocate a new vector, based on type information.
 * type:        A fully specified C type.
 * return:      A pointer to the memory region, or NULL.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#define evinit(type) evini(sizeof(type), EV_INIT_COUNT)


/**
 * Easy allocate a new vector with slot sizes as given
 * slt_size:    The size of each slot in the vector typically the size of
 *              the type that is being stored.
 * return:      A pointer to the memory region, or NULL.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#define evinisz(sz)  evini(sz, EV_INIT_COUNT)


/**
 * Allocate a new vector and initialize it.
 * slt_size:    The size of each slot in the vector typically the size of
 *              the type that is being stored.
 * count:       The number of initial elements (of size slt_size) to be
 *              allocated. This should be set to the lower bound of the expected
 *              number of items (which could be zero).
 * return:      A pointer to the memory region, or NULL.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evini(size_t slt_size, size_t count);


/**
 * Easy push a new value onto the tail of a vector. If the vector is NULL,
 * memory will be automatically allocated for INIT_COUNT elements, based on the
 * object size as returned by sizeof(obj). If the memory backing the vector is
 * too small, memory will be reallocated to grow the vector by the the
 * GROWTH_FACTOR. e.g. 16B with a GROWTH_FACTOR=2 will grow to 32B.
 *
 * The reason for this wrapper macro is to make it easy to push literal values.
 *
 * Note: This function is only available on compilers supporting GNU C
 * extensions (specifically the typeof() operator)
 *
 * vec:         Pointer to type of object that is (or will become) the vector,
 *              eg. int* for a vector of ints.
 * obj:         The value to push into the vector.
 * return:      A pointer to the memory region, or NULL.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 * quirks:      This macro works fine for all literals expect string literals,
 *              for which you probably want to store the char* pointer, but this
 *              doesn't really exist. For these types, you'll need to use the
 *              explicit evpush function.
 */
#ifdef __GNUC__
#define evpsh(vec, obj) do { \
         __extension__ __typeof__(obj) __OBJ__ = obj; \
         vec = evpush(vec, &__OBJ__, sizeof(__OBJ__)); \
     }while(0)
#else
#define evpsh(vec, obj) do { \
         vec = evpush(vec, &obj, sizeof(obj)); \
     }while(0
#endif


/**
 * Push a new value onto the vector tail. The if the vector is NULL, memory
 * will be automatically allocated for INIT_COUNT elements, based on the
 * object size supplied.
 *
 * If the memory backing the vector is too small, memory will be reallocated to
 * grow the vector by the the GROWTH_FACTOR. e.g. 16B with a GROWTH_FACTOR=2
 * will grow to 32B.
 * vec:         pointer to type of object that is (or will become) the vector,
 *              eg. int* for a vector of ints.
 * obj:         pointer to the value to push into the vector.
 * obj_size:    the size of the value to be pushed into the vector.
 * return:      A pointer to the memory region, or NULL.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evpush(void* vec, void* obj, size_t obj_size);

/**
 * Macro to help iterate over each element of the vector, putting a pointer to
 * the element in var.
 *
 * **Note** this pointer is only valid until the next vector operation.
 * A vector operation (such as `evpsh()`) may cause a memory reallocation which
 * can make this pointer undefined.
 *
 * vec:         pointer to type of object that is (or will become) the vector,
 *              eg. int* for a vector of ints.
 * ivar:        name of the iterator variable to use
 * return:      A pointer to the memory region, or NULL.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#define eveach(vec,ivar) \
    for(typeof(vec) ivar = evhead(vec); ivar; ivar = evnext(vec))


/**
 * Return a the pointer to the first slot in the vector.
 *
 * **Note** this pointer is only valid until the next vector operation.
 * A vector operation (such as `evpsh()`) may cause a memory reallocation which
 * can make this pointer undefined.
 *
 * vec:         Pointer to the vector
 * return:      A pointer to the first slot in the vector, or NULL
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evhead(void* vec);

/**
 * Return a the pointer next value after the head (if evhead() was last called ),
 * or next value after the last call to evnext(). It is invalid to call evnext()
 * without first calling evhead(). When there are no more elements in the vector,
 * evnext() returns NULL;
 *
 * **Note** this pointer is only valid until the next vector operation.
 * A vector operation (such as `evpsh()`) may cause a memory reallocation which
 * can make this pointer undefined.
 *
 * vec:         Pointer to the vector
 * return:      A pointer to next slot in the vector, or NULL
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evnext(void* vec);



/**
 * Return a the pointer to the last slot in the vector.
 *
 * **Note** this pointer is only valid until the next vector operation.
 * A vector operation (such as `evpsh()`) may cause a memory reallocation which
 * can make this pointer undefined.
 *
 * vec:         Pointer to the vector
 * return:      A pointer to the last slot in the vector, or NULL
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evtail(void* vec);


/**
 * Get the number of items in the vector.
 * vec:         Pointer to the vector
 * return:      The number of objects in the vector.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evcnt(void* vec);


/**
 * Return a the pointer to the slot at a given index
 *
 * **Note** this pointer is only valid until the next vector operation.
 * A vector operation (such as `evpsh()`) may cause a memory reallocation which
 * can make this pointer undefined.
 *
 * vec:         Pointer to the vector
 * idx:         The index value. Cannot be <0 or greater than the object count
 * return:      Pointer to the value.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evidx(void* vec, size_t idx);


/**
 * Free the memory used to hold the vector and its accounting.
 * vec:         Pointer to the vector
 * return:      NULL. Use vec = evfree(vec) to ensure there are no dangling
 *              pointers.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evfree(void* vec);


/**
 * Remove the last value from the vector tail.
 * vec:         Pointer to the vector
 * return:      None
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#if defined EV_FPOP || defined EV_FALL
void evpop(void *vec);
#endif


/**
 * Remove a value from the vector at the given index
 * vec:         Pointer to the vector
 * idx:         The index into the vector. Must be >0 and < count.
 * return:      None
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#if defined EV_FDEL || defined EV_FALL
void evdel(void *vec, size_t idx);
#endif


#if defined EV_FMEMSZ  || defined EV_FALL
/**
 * Get the current size of the vector.
 * vec:         Pointer to the vector
 * return:      The current number of slots in the vector.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evvsz(void* vec);

/**
 * Get the amount of memory currently used to store the vector including unused
 * slots.
 * vec:         Pointer to the vector
 * return:      The amount of memory currently used to store the vector.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evvmem(void* vec);


/**
 * Get the amount of memory currently used to store objects in the vector
 * vec:         Pointer to the vector
 * return:      The amount of memory currently used to store objects.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evomem(void* vec);


/**
 * Get the total memory used by the vector.
 * vec:         Pointer to the vector
 * return:      The total amount of memory consumed by the vector including
 *              accounting overheads.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evtmem(void* vec);

#endif

/**
 * Sort the elements of the vector
 * vec:         Pointer to the vector
 * compar:      Function pointer which implements the comparison function.
 *              This function returns +ve if a > b, -ve if a < b and 0 if a==b.
 * return:      None. The vector will be sorted if this function succeeds.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#if defined EV_FSORT  | defined EV_FALL
void evsort(void* vec, int (*compar)(const void* a, const void* b));
#endif

/**
 * Create a new vector and copy the contents of the source vector into it.
 * src:         Pointer to the source vector
 * return:      A new vector with the same contents as the source.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#if defined EV_FCOPY  | defined EV_FALL
void* evcpy(void* src);
#endif

/*
 * Implementation!
 * ============================================================================
 */


#if EV_PEDANTIC
#define ifp(p,e) if(p){e}
#else
#define ifp(p,e)
#endif


#ifndef EV_HONLY
/*
 * When building with multiple .c files, you only need one instance of the EV
 * function implementations. For all other instances, you only need the forwad
 * declaration headers. In these cases, be sure to define EV_HONLY before
 * including evec.h. This will keep the compiler happy for you. e.g.
 *
 * #define EV_FHONLY
 * #include "evec.h"
 */


//Round up to the nearest long. Just a bit of memory safety paranoia
typedef long align;
#define EV_HDR_BYTES (( (sizeof(evhd_t) + sizeof(align) - 1) / sizeof(align)) * sizeof(align))
#define EV_HDR(v) ((evhd_t*)( ((char*)v) - EV_HDR_BYTES ))
#define EV_VER 1.1

/*
 * Magic values are included so that it's easy to spot the memory segment in
 * a hexdump, and to protect from any violations that might happen by accident
 */
#define EV_MAGIC1 "EVMAGIC"
#define EV_MAGIC2 "MAGICEV"
typedef struct {
    char magic1[8];
    int64_t slt_size;
    int64_t obj_count;
    int64_t slt_count;
    int64_t index;
    char magic2[8];
} evhd_t;


#define EV_DU_HDR(hdr) _evdumphdr(__LINE__, __FILE__, __FUNCTION__, hdr)
void _evdumphdr(int ln, char* fn, const char* fu, evhd_t* hdr)
{
    dprintf(STDERR_FILENO,"[HEADER :   %s:%i:%s()] ", basename(fn), ln, fu);
    dprintf(STDERR_FILENO,"magic1: %s, ", hdr->magic1);
    dprintf(STDERR_FILENO,"slt_size: %" PRId64 ", ", hdr->slt_size);
    dprintf(STDERR_FILENO,"slt_count: %" PRId64 ", ", hdr->slt_count);
    dprintf(STDERR_FILENO,"obj_count: %" PRId64 ", ", hdr->obj_count);
    dprintf(STDERR_FILENO,"magic2: %s\n", hdr->magic2);
}

/*
 * This debugging code liberally borrowed and adapted from libchaste by
 * M.Grosvenor BSD 3 clause license. https://github.com/mgrosvenor/libchaste
 */
typedef enum {
    EV_MSG_DBG,
    EV_MSG_WARN,
    EV_MSG_ERR,
} evdbg_e;

#define EV_FAIL( /*format, args*/...)  everr_helper(__VA_ARGS__, "")
#define everr_helper(format, ...) _evmsg(EV_MSG_ERR, __LINE__, __FILE__, __FUNCTION__, format, __VA_ARGS__ )
#define EV_WARN( /*format, args*/...)  evwarn_helper(__VA_ARGS__, "")
#define evwarn_helper(format, ...) _evmsg(EV_MSG_WARN,__LINE__, __FILE__, __FUNCTION__, format, __VA_ARGS__ )

#if EV_DEBUG
#define EV_DBG( /*format, args*/...)  evdebug_helper(__VA_ARGS__, "")
    #define evdebug_helper(format, ...) _evmsg(EV_MSG_DBG,__LINE__, __FILE__, __FUNCTION__, format, __VA_ARGS__ )
#else
#define EV_DBG( /*format, args*/...)
#endif


static inline void _evmsg(evdbg_e mode, int ln, char* fn, const char* fu, const char* msg, ... )
{
    va_list args;
    va_start(args,msg);
    char* mode_str = NULL;
    switch(mode){
        case EV_MSG_ERR:   mode_str = "Error  :"; break;
        case EV_MSG_DBG:   mode_str = "Debug  :"; break;
        case EV_MSG_WARN:  mode_str = "Warning:"; break;
    }
    dprintf(STDERR_FILENO,"[%s %s:%i:%s()]  ", mode_str, basename(fn), ln, fu);
    vdprintf(STDERR_FILENO,msg,args);

    if(mode == EV_MSG_ERR && EV_HARD_EXIT){
        exit(0xDEAD);
    }
    va_end(args);
}


void* evini(size_t slt_size, size_t count)
{
    size_t store_bytes  = count * slt_size;
    size_t full_bytes = EV_HDR_BYTES + store_bytes;

    evhd_t *hdr = (evhd_t*)malloc(full_bytes);
    ifp(!hdr,
        EV_FAIL("No memory to init vector with %" PRId64 "B\n", full_bytes);
                return NULL;
    );


    memset(hdr,0x00,full_bytes);

    memcpy(hdr->magic1,EV_MAGIC1,sizeof(hdr->magic1));
    hdr->slt_size   = slt_size;
    hdr->slt_count  = count;
    hdr->obj_count  = 0;
    memcpy(hdr->magic2,EV_MAGIC2,sizeof(hdr->magic2));

    void *vec_start = (char*)hdr + EV_HDR_BYTES;
    return vec_start;
}

//Check that the EV header is sane
int _evhdrcheck(evhd_t* hdr)
{
    if(strncmp(hdr->magic1, EV_MAGIC1, sizeof(EV_MAGIC1)) != 0){
        EV_FAIL("Header magic 1 should be '%s' but found '%.*s'\n", EV_MAGIC1, sizeof(EV_MAGIC1), hdr->magic1);
        return -1;
    };
    if(strncmp(hdr->magic2, EV_MAGIC2, sizeof(EV_MAGIC2)) != 0){
        EV_FAIL("Header magic 2 should be '%s' but found '%.*s'\n", EV_MAGIC2, sizeof(EV_MAGIC2), hdr->magic2);
        return -1;
    };

    if(hdr->obj_count < 0){
        EV_FAIL("Object count cannot be less than zero!\n");
        return -1;
    }

    if(hdr->index < 0){
        EV_FAIL("Index value cannot be less than zero!\n");
        return -1;
    }

    if(hdr->slt_count < 0){
        EV_FAIL("Slot count cannot be less than zero\n");
        return -1;
    }

    if(hdr->obj_count > hdr->slt_count){
        EV_FAIL("More items in vector (%" PRId64 ") than there is space (%" PRId64 ")\n",
                hdr->obj_count,
                hdr->slt_count);
        return -1;
    }

    if(hdr->slt_size < 0){
        EV_FAIL("Slot size cannot be less than zero\n");
        return -1;
    }

    return 0;
}

//Internal function, grow the vector backing store memory
void* _evgrow(void* vec)
{
    ifp(!vec,
        EV_FAIL("Cannot grow an empty vector!\n");
    );

    evhd_t* hdr = EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
                return NULL;
    );


    const size_t storage_bytes      = hdr->slt_size * hdr->slt_count;
    const size_t new_storage_bytes  = hdr->slt_count ? storage_bytes * EV_GROWTH_FACTOR :
                                      EV_INIT_COUNT * hdr->slt_size;
    const size_t full_bytes         = EV_HDR_BYTES + new_storage_bytes;

    hdr = realloc(hdr, full_bytes);
    if (!hdr){
        EV_FAIL("No memory to grow vector up to %" PRId64 "B\n", full_bytes);
        return NULL;
    }

    memset((char*)hdr + EV_HDR_BYTES + storage_bytes, 0x00, new_storage_bytes - storage_bytes);

    hdr->slt_count  = hdr->slt_count ? hdr->slt_count * EV_GROWTH_FACTOR : EV_INIT_COUNT;

    void *vec_start = (char*)hdr + EV_HDR_BYTES;

    return vec_start;
}


void* evpush(void* vec, void* obj, size_t obj_size)
{
    void* result = vec;
    ifp(!vec,
    //Get some memory
        result = evinisz(obj_size);
    );

    evhd_t* hdr = EV_HDR(result);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
                return NULL;
    );

    //Sanity check
    ifp(obj_size > hdr->slt_size,
        EV_FAIL("Object size (%" PRId64 ") is larger than there is space (%" PRId64 ")\n",
                obj_size,
                hdr->slt_size);
                return NULL;
    );

    //Enough space?
    if(hdr->obj_count == hdr->slt_count){
        //Get some more
        result = _evgrow(result);
        hdr = EV_HDR(result);
    }

    void* next_obj = ((char*)result) + hdr->slt_size * hdr->obj_count;
    memcpy(next_obj,obj,obj_size);
    hdr->obj_count++;

    return result;
}


size_t evcnt(void* vec)
{
    if(!vec){
        return 0;
    };

    evhd_t *hdr =  EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
                return -1;
    );

    return hdr->obj_count;
}


void* evidx(void* vec, size_t idx)
{
    ifp(!vec,
        EV_FAIL("Cannot get index of a NULL vector\n");
                return NULL;
    );

    evhd_t *hdr =  EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
                return NULL;
    );

    ifp(hdr->obj_count == 0,
        EV_FAIL("Cannot get index of empty vector\n");
                return NULL;
    );

    ifp(idx < 0,
        EV_FAIL("Index cannot be <0 (idx=%" PRId64 ")\n", idx);
    );

    ifp(idx > hdr->obj_count - 1,
        EV_FAIL("Index cannot be greater than number of objects (idx=%" PRId64 " > %" PRId64 ")\n" ,
                idx,
                hdr->obj_count -1);
    );


    return (char*)vec + hdr->slt_size * idx;
}


void* evtail(void* vec)
{
    return evidx(vec,evcnt(vec) -1);
}

void* evhead(void* vec)
{
    ifp(!vec,
    return NULL;
    );

    evhd_t *hdr =  EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
        return NULL;
    );

    ifp(hdr->obj_count == 0,
        return NULL;
    );

    hdr->index = 0;

    return evidx(vec,hdr->index);
}

void* evnext(void* vec)
{
    ifp(!vec,
        EV_FAIL("Cannot get next item in a NULL vector\n");
                return NULL;
    );

    evhd_t *hdr =  EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
                return NULL;
    );

    ifp(hdr->obj_count == 0,
        EV_FAIL("Cannot get next item in an empty vector\n");
                return NULL;
    );

    hdr->index++;
    if(hdr->index >= hdr->obj_count){
        return NULL;
    }

    return evidx(vec,hdr->index);

}

void* evfree(void* vec)
{
    if(!vec){
        return NULL;
    };

    if(vec){
        evhd_t *hdr =  EV_HDR(vec);
        ifp(_evhdrcheck(hdr),
            EV_FAIL("Header sanity check failed\n");
                    return NULL;
        );
        free(hdr);
    }

    return NULL;
}



#if defined EV_FPOP || defined EV_FALL
void evpop(void *vec)
{
    ifp(!vec,
        //Uh ohhh....
        EV_FAIL("Cannot pop a NULL vector\n");
        return;
    );

    evhd_t* hdr =  EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
        return;
    );

    if(hdr->obj_count)
        hdr->obj_count--;

}
#endif


#if defined EV_FDEL || defined EV_FALL
void evdel(void *vec, size_t idx)
{
    ifp(!vec,
        //Uh ohhh....
        EV_FAIL("Cannot pop an empty vector\n");
        return;
    );

    evhd_t* hdr = EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
        return;
    );

    if(hdr->obj_count == 0){
        //Nothing to delete
        return;
    }

    //Sanity check
    ifp(idx < 0,
        EV_FAIL("Vector index cannot be less than 0\n");
        return;
    )

    ifp(idx > hdr->obj_count - 1,
        EV_FAIL("Vector index (%lu) too large (%" PRId64")\n", idx, hdr->obj_count -1);
        return;
    );

    void* curr_obj = (char*)vec + hdr->slt_size * (idx + 0);
    void* next_obj = (char*)vec + hdr->slt_size * (idx + 1);
    const size_t to_move = hdr->slt_size * (hdr->obj_count - idx);

    memcpy(curr_obj,next_obj,to_move);

    hdr->obj_count--;
}
#endif


#if defined EV_FMEMSZ  || defined EV_FALL

size_t evvsz(void* vec)
{
    ifp(!vec,
        EV_FAIL("Cannot get size of a NULL vector\n");
        return -1;
    );

    evhd_t *hdr = EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
        return -1;
    );

    return hdr->slt_count;
}


size_t evvmem(void* vec)
{
    ifp(!vec,
        EV_FAIL("Cannot get size of a NULL vector\n");
        return -1;
    );

    evhd_t *hdr = EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
        return -1;
    );

    return hdr->slt_count * hdr->slt_size;
}


size_t evomem(void* vec)
{
    ifp(!vec,
        EV_FAIL("Cannot get size of a NULL vector\n");
        return -1;
    );

    evhd_t *hdr = EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
        return -1;
    );

    return hdr->obj_count * hdr->slt_size;
}


size_t evtmem(void* vec)
{
    ifp(!vec,
        EV_FAIL("Cannot get size of a NULL vector\n");
        return -1;
    );

    return evvmem(vec) + EV_HDR_BYTES;
}

#endif


#if defined EV_FSORT  | defined EV_FALL
void evsort(void* vec, int (*compar)(const void* a, const void* b))
{
    ifp(!vec,
        EV_FAIL("Cannot sort a NULL vector\n");
        return;
    );

    evhd_t *hdr =  EV_HDR(vec);
    ifp(_evhdrcheck(hdr),
        EV_FAIL("Header sanity check failed\n");
        return;
    );

    qsort(vec,hdr->obj_count,hdr->slt_size,compar);
}
#endif


#if defined EV_FCOPY  | defined EV_FALL
void* evcpy(void* src)
{
    ifp(!src,
        EV_FAIL("Cannot copy a NULL vector\n");
        return NULL;
    );
    evhd_t *src_hdr = EV_HDR(src);
    ifp(_evhdrcheck(src_hdr),
        EV_FAIL("Header sanity check failed\n");
        return NULL;
    );


    void* result = NULL;
    result = evini(src_hdr->slt_size, src_hdr->slt_count);
    if(!result){
        EV_FAIL("Could not create new vector memory to copy into\n");
        return NULL;
    }
    evhd_t *res_hdr = EV_HDR(result);

    memcpy(res_hdr,src_hdr,EV_HDR_BYTES + src_hdr->slt_size * src_hdr->obj_count);

    return result;
}
#endif

#endif /* EV_HONLY */

#endif /* EVH_ */
