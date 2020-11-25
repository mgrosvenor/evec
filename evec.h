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

#ifndef EV_HARD_EXIT
#define EV_HARD_EXIT       1 //Fail to exit(0xDEAD).
#endif

#ifndef EV_INIT_COUNT
#define EV_INIT_COUNT      8 //Start with 8 slots
#endif

#ifndef EV_GROWTH_FACTOR
#define EV_GROWTH_FACTOR   2 //Grow by a factor of x when space runs out
#endif

//Round up to the nearest long. Just a bit of memory safety paranoia
typedef long align;
#define EV_HDR_BYTES (( (sizeof(evhd_t) + sizeof(align) - 1) / sizeof(align)) * sizeof(align))
#define EV_HDR(v) ((evhd_t*)( ((char*)v) - EV_HDR_BYTES ))
#define VER 1.0

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
    char magic2[8];
} evhd_t;


#define EV_DU_HDR(hdr) _evdumphdr(__LINE__, __FILE__, __FUNCTION__, hdr)
void _evdumphdr(int ln, char* fn, const char* fu, evhd_t* hdr){
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

#ifndef NDEBUG
    #define EV_WARN( /*format, args*/...)  evwarn_helper(__VA_ARGS__, "")
    #define evwarn_helper(format, ...) _evmsg(EV_MSG_WARN,__LINE__, __FILE__, __FUNCTION__, format, __VA_ARGS__ )
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
void* evini(size_t slt_size, size_t count)
{
    size_t store_bytes  = count * slt_size;
    size_t full_bytes = EV_HDR_BYTES + store_bytes;

    evhd_t *hdr = (evhd_t*)malloc(full_bytes);
    if (!hdr){
        EV_FAIL("No memory to init vector with %" PRId64 "B\n", full_bytes);
        return NULL;
    }
    memset(hdr,0x00,full_bytes);

    memcpy(hdr->magic1,EV_MAGIC1,sizeof(hdr->magic1));
    hdr->slt_size   = slt_size;
    hdr->slt_count  = count;
    hdr->obj_count  = 0;
    memcpy(hdr->magic2,EV_MAGIC2,sizeof(hdr->magic2));

    void *vec_start = (char*)hdr + EV_HDR_BYTES;
    return vec_start;
}


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


//Internal function, grow the vector backing store memory
void* _evgrow(void* vec)
{
    if(!vec){
        EV_FAIL("Cannot grow an empty vector!\n");
    }

    evhd_t* hdr = EV_HDR(vec);
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
 * extensions
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
void* evpush(void* vec, void* obj, size_t obj_size)
{
    void* result = vec;
    if (!vec) {
        //Get some memory
        result = evinisz(obj_size);
    }

    evhd_t* hdr = EV_HDR(result);

    //Sanity check
    if(hdr->obj_count > hdr->slt_count){
        //Uh ohhh....
        EV_FAIL("More items in vector (%" PRId64 ") than there is space (%" PRId64 ")\n",
                hdr->obj_count,
                hdr->slt_count);
        return NULL;
    }

    if(obj_size > hdr->slt_size){
        EV_FAIL("Object size (%" PRId64 ") is larger than there is space (%" PRId64 ")\n",
                obj_size,
                hdr->slt_size);
        return NULL;
    }

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


/**
 * Get the number of items in the vector.
 * vec:         Pointer to the vector
 * return:      The number of objects in the vector.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evcnt(void* vec)
{
    if(!vec){
        EV_FAIL("Cannot count a NULL vector\n");
        return -1;
    }

    evhd_t *hdr =  EV_HDR(vec);
    return hdr->obj_count;
}

/**
 * Return a the pointer to the slot at a given index.
 * vec:         Pointer to the vector
 * idx:         The index value. Cannot be <0 or greater than the object count
 * return:      Pointer to the value.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evidx(void* vec, size_t idx)
{
    if(!vec){
        EV_FAIL("Cannot get index of a NULL vector\n");
        return NULL;
    }

    evhd_t *hdr =  EV_HDR(vec);

    if(hdr->obj_count == 0){
        EV_FAIL("Cannot get index of empty vector\n");
        return NULL;
    }

    if(idx < 0){
        EV_FAIL("Index cannot be <0 (idx=%" PRId64 ")\n", idx);
    }

    if(idx > hdr->obj_count - 1){
        EV_FAIL("Index cannot be greater than number of objects (idx=%" PRId64 " > %" PRId64 ")\n" ,
                idx,
                hdr->obj_count -1);
    }


    return (char*)vec + hdr->slt_size * idx;
}


/**
 * Free the memory used to hold the vector and its accounting.
 * vec:         Pointer to the vector
 * return:      NULL. Use vec = evfree(vec) to ensure there are no dangling
 *              pointers.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
void* evfree(void* vec)
{
    if(!vec){
        EV_FAIL("Cannot free empty vector!\n");
        return NULL;
    }

    if(vec){
        evhd_t *hdr =  EV_HDR(vec);
        free(hdr);
    }

    return NULL;
}



/**
 * Remove the last value from the vector tail.
 * vec:         Pointer to the vector
 * return:      None
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#if defined EV_FPOP || defined EV_FALL
void evpop(void *vec)
{
    if(!vec) {
        //Uh ohhh....
        EV_FAIL("Cannot pop a NULL vector\n");
        return;
    }

    evhd_t* hdr =  EV_HDR(vec);

    if(hdr->obj_count)
        hdr->obj_count--;

}
#endif


/**
 * Remove a value from the vector at the given index
 * vec:         Pointer to the vector
 * idx:         The index into the vector. Must be >0 and < count.
 * return:      None
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#if defined EV_FDEL || defined EV_FALL
void evdel(void *vec, size_t idx)
{
    if (!vec) {
        //Uh ohhh....
        EV_FAIL("Cannot pop an empty vector\n");
        return;
    }

    evhd_t* hdr = EV_HDR(vec);

    if(hdr->obj_count == 0){
        //Nothing to delete
        return;
    }

    //Sanity check
    if(hdr->obj_count > hdr->slt_count){
        //Uh ohhh....
        EV_FAIL("BUG! More items in vector (%" PRId64 ") than there is space (%" PRId64 ")\n",
                hdr->obj_count,
                hdr->slt_count);
        return;
    }

    //More sanity checking
    if(idx < 0){
        EV_FAIL("Vector index cannot be less than 0\n");
        return;
    }
    else if (idx > hdr->obj_count - 1){
        EV_FAIL("Vector index (%lu) too large (%" PRId64")\n", idx, hdr->obj_count -1);
        return;
    }

    if(idx == hdr->obj_count - 1){
        //Removing the last item. Fast exit.
        evpop(vec);
        return;
    }

    void* curr_obj = (char*)vec + hdr->slt_size * (idx + 0);
    void* next_obj = (char*)vec + hdr->slt_size * (idx + 1);
    const size_t to_move = hdr->slt_size * (hdr->obj_count - idx);

    memcpy(curr_obj,next_obj,to_move);

    hdr->obj_count--;
}
#endif


#if defined EV_FMEMSZ  || defined EV_FALL
/**
 * Get the current size of the vector.
 * vec:         Pointer to the vector
 * return:      The current number of slots in the vector.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evvsz(void* vec)
{
    if(!vec){
        EV_FAIL("Cannot get size of a NULL vector\n");
        return -1;
    }

    evhd_t *hdr = EV_HDR(vec);
    return hdr->slt_count;
}


/**
 * Get the amount of memory currently used to store the vector including unused
 * slots.
 * vec:         Pointer to the vector
 * return:      The amount of memory currently used to store the vector.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evvmem(void* vec)
{
    if(!vec){
        EV_FAIL("Cannot get size of a NULL vector\n");
        return -1;
    }

    evhd_t *hdr = EV_HDR(vec);
    return hdr->slt_count * hdr->slt_size;
}


/**
 * Get the amount of memory currently used to store objects in the vector
 * vec:         Pointer to the vector
 * return:      The amount of memory currently used to store objects.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evomem(void* vec)
{
    if(!vec){
        EV_FAIL("Cannot get size of a NULL vector\n");
        return -1;
    }

    evhd_t *hdr = EV_HDR(vec);
    return hdr->obj_count * hdr->slt_size;
}

/**
 * Get the total memory used by the vector.
 * vec:         Pointer to the vector
 * return:      The total amount of memory consumed by the vector including
 *              accounting overheads.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
size_t evtmem(void* vec)
{
    if(!vec){
        EV_FAIL("Cannot get size of a NULL vector\n");
        return -1;
    }

    return evvmem(vec) + EV_HDR_BYTES;
}

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
void evsort(void* vec, int (*compar)(const void* a, const void* b))
{
    if(!vec){
        EV_FAIL("Cannot sort a NULL vector\n");
        return;
    }

    evhd_t *hdr =  EV_HDR(vec);
    qsort(vec,hdr->obj_count,hdr->slt_size,compar);
}
#endif



/**
 * Create a new vector and copy the contents of the source vector into it.
 * src:         Pointer to the source vector
 * return:      A new vector with the same contents as the source.
 * failure:     If EV_HARD_EXIT is enabled, this function may cause exit();
 */
#if defined EV_FCOPY  | defined EV_FALL
void* evcpy(void* src)
{
    if(!src){
        EV_FAIL("Cannot copy a NULL vector\n");
        return NULL;
    }
    evhd_t *src_hdr = EV_HDR(src);
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


#endif /* EVH_ */
