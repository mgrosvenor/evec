
/*
 * Really simple vector
 *
 * This is a low performance vector type for C programs where simplicity is the
 * key concern. It's designed for situations like options parsing where building
 * up an unknown amount of state is required, but it is likely to be small.
 *
 * The neat thing about this vector is that it maintains array semantics by
 * hiding state near the allocated memory. Lots of macro magic makes it easy
 * to use.
 *
 *  Created on: 18 Nov 2020
 *      Author: Matthew Grosvenor
 */

#ifndef RSVEC_H_
#define RSVEC_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h>
#include <ctype.h>
#include <libgen.h>
#include <unistd.h>


#define RSV_INIT_COUNT      8 //Start with 8 objects
#define RSV_GROWTH_FACTOR   2 //Grow by a factor of x when space runs out

//Round up to the nearest int. Just a bit of memory safety paranoia
typedef uint64_t u64;
#define RSV_HDR_BYTES (( (sizeof(rsv_hd_t) + sizeof(u64) - 1) / sizeof(u64)) * sizeof(u64))
#define RSV_HDR(v) ((rsv_hd_t*)( ((char*)v) - RSV_HDR_BYTES ))

#define RSV_MAGIC1 "RSVMAGIC"
#define RSV_MAGIC2 "MAGICRSV"

typedef struct {
    char magic1[8];
    int64_t slt_size;
    int64_t obj_count;
    int64_t slt_count;
    char magic2[8];
} rsv_hd_t;

/*
 * This code liberally borrowed from libchaste by M.Grosvenor BSD 3 clause
 * license. https://github.com/mgrosvenor/libchaste
 */

typedef enum {
    RSV_MSG_DBG,
    RSV_MSG_WARN,
    RSV_MSG_ERR,
} rsv_dbg_e;


#define FAIL( /*format, args*/...)  rsv_err_helper(__VA_ARGS__, "")
#define rsv_err_helper(format, ...) _rsv_msg(RSV_MSG_ERR, __LINE__, __FILE__, __FUNCTION__, format, __VA_ARGS__ )

#ifndef NDEBUG
    #define WARN( /*format, args*/...)  rsv_warn_helper(__VA_ARGS__, "")
    #define rsv_warn_helper(format, ...) _rsv_msg(RSV_MSG_WARN,__LINE__, __FILE__, __FUNCTION__, format, __VA_ARGS__ )
    #define DBG( /*format, args*/...)  rsv_debug_helper(__VA_ARGS__, "")
    #define rsv_debug_helper(format, ...) _rsv_msg(RSV_MSG_DBG,__LINE__, __FILE__, __FUNCTION__, format, __VA_ARGS__ )
#else
    #define DBG( /*format, args*/...)
#endif

static inline void _rsv_msg(rsv_dbg_e mode, int ln, char* fn, const char* fu, const char* msg, ... )
{
    va_list args;
    va_start(args,msg);
    char* mode_str = NULL;
    switch(mode){
        case RSV_MSG_ERR:   mode_str = "Error  :"; break;
        case RSV_MSG_DBG:   mode_str = "Debug  :"; break;
        case RSV_MSG_WARN:  mode_str = "Warning:"; break;
    }
    dprintf(STDERR_FILENO,"[%s %s:%i:%s()]  ", mode_str, basename(fn), ln, fu);
    vdprintf(STDERR_FILENO,msg,args);

    if(mode == RSV_MSG_ERR){
        exit(0xDEAD);
    }
    va_end(args);
}


//Some useful shortenings
#define rsv_initt(type) rsv_init(sizeof(type), RSV_INIT_COUNT)
#define rsv_initl(len)  rsv_init(len, RSV_INIT_COUNT)
//Allocate a new vector storage and initialise
void* rsv_init(size_t slt_size, size_t count)
{
    size_t store_bytes  = RSV_INIT_COUNT * slt_size;
    size_t full_bytes = RSV_HDR_BYTES + store_bytes;

    rsv_hd_t *hdr = (rsv_hd_t*)malloc(full_bytes);
    if (!hdr){
        FAIL("No memory to init vector with %" PRId64 "B\n", full_bytes);
    }
    memset(hdr,0x00,full_bytes);

    memcpy(hdr->magic1,RSV_MAGIC1,sizeof(hdr->magic1));
    hdr->slt_size   = slt_size;
    hdr->slt_count  = count;
    hdr->obj_count  = 0;
    memcpy(hdr->magic2,RSV_MAGIC2,sizeof(hdr->magic2));

    void *vec_start = (char*)hdr + RSV_HDR_BYTES;
    return vec_start;
}


void* _rsv_grow(void* vec)
{
    if(!vec){
        FAIL("Cannot grow an empty vector!\n");
    }

    rsv_hd_t* hdr = RSV_HDR(vec);
    const size_t storage_bytes  = hdr->slt_size * hdr->slt_count;
    const size_t new_storage_bytes  = hdr->slt_size * hdr->slt_count * RSV_GROWTH_FACTOR;
    const size_t full_bytes = RSV_HDR_BYTES + new_storage_bytes;

    hdr = realloc(hdr, full_bytes);
    if (!hdr){
        FAIL("No memory to grow vector up to %" PRId64 "B\n", full_bytes);
    }

    memset((char*)hdr + RSV_HDR_BYTES + storage_bytes, 0x00, new_storage_bytes - storage_bytes);

    hdr->slt_count  *= RSV_GROWTH_FACTOR;

    void *vec_start = (char*)hdr + RSV_HDR_BYTES;

    return vec_start;
}


#define rsv_psh(vec, obj) do { \
        typeof(obj) __OBJ__ = obj; \
        vec = rsv_push(vec, &__OBJ__, sizeof(__OBJ__)); \
    }while(0)

void* rsv_push(void* vec, void* obj, size_t obj_size)
{
    void* result = vec;
    if (!vec) {
        //Get some memory
        result = rsv_init(obj_size,RSV_INIT_COUNT);
    }

    rsv_hd_t* hdr = RSV_HDR(result);

    //Sanity check
    if(hdr->obj_count > hdr->slt_count){
        //Uh ohhh....
        FAIL("More items in vector (%" PRId64 ") than there is space (%" PRId64 ")\n",
                hdr->obj_count,
                hdr->slt_count);

        return vec; //Do this so we can free memory later
    }

    if(obj_size > hdr->slt_size){
        FAIL("Object size (%" PRId64 ") is larger than there is space (%" PRId64 ")\n",
                obj_size,
                hdr->slt_size);
    }

    //Enough space?
    if(hdr->obj_count == hdr->slt_count){
        //Get some more
        result = _rsv_grow(result);
        hdr = RSV_HDR(result);
    }

    void* next_obj = ((char*)result) + hdr->slt_size * hdr->obj_count;
    memcpy(next_obj,obj,obj_size);
    hdr->obj_count++;
    return result;
}


void rsv_pop(void *vec)
{
    if(!vec) {
        //Uh ohhh....
        FAIL("Cannot pop a NULL vector\n");
        return;
    }

    rsv_hd_t* hdr =  RSV_HDR(vec);

    if(hdr->obj_count)
        hdr->obj_count--;

}


void rsv_del(void *vec, size_t idx)
{
    if (!vec) {
        //Uh ohhh....
        FAIL("Cannot pop an empty vector\n");
    }

    rsv_hd_t* hdr = RSV_HDR(vec);

    if(hdr->obj_count == 0){
        //Nothing to delete
        return;
    }

    //Sanity check
    if(hdr->obj_count > hdr->slt_count){
        //Uh ohhh....
        FAIL("BUG! More items in vector (%" PRId64 ") than there is space (%" PRId64 ")\n",
                hdr->obj_count,
                hdr->slt_count);
        return;
    }

    //More sanity checking
    if(idx < 0){
        FAIL("Vector index cannot be less than 0\n");
    }
    else if (idx > hdr->obj_count - 1){
        FAIL("Vector index (%lu) too large (%" PRId64")\n", idx, hdr->obj_count -1);
    }

    if(idx == hdr->obj_count - 1){
        //Removing the last item. Fast exit.
        return rsv_pop(vec);
    }

    void* curr_obj = (char*)vec + hdr->slt_size * (idx + 0);
    void* next_obj = (char*)vec + hdr->slt_size * (idx + 1);
    const size_t to_move = hdr->slt_size * (hdr->obj_count - idx);

    memcpy(curr_obj,next_obj,to_move);

    hdr->obj_count--;

}

size_t rsv_cnt(void* vec)
{
    if(!vec){
        FAIL("Cannot count a NULL vector\n");
    }

    rsv_hd_t *hdr =  RSV_HDR(vec);
    return hdr->obj_count;
}

size_t rsv_vsz(void* vec)
{
    if(!vec){
        FAIL("Cannot get size of a NULL vector\n");
    }

    rsv_hd_t *hdr = RSV_HDR(vec);
    return hdr->slt_count;
}

size_t rsv_vmem(void* vec)
{
    if(!vec){
        FAIL("Cannot get size of a NULL vector\n");
    }

    rsv_hd_t *hdr = RSV_HDR(vec);
    return hdr->slt_count * hdr->slt_size;
}


size_t rsv_omem(void* vec)
{
    if(!vec){
        FAIL("Cannot get size of a NULL vector\n");
    }

    rsv_hd_t *hdr = RSV_HDR(vec);
    return hdr->obj_count * hdr->slt_size;
}

size_t rsv_tmem(void* vec)
{
    if(!vec){
        FAIL("Cannot get size of a NULL vector\n");
    }

    return rsv_vmem(vec) + RSV_HDR_BYTES;
}

void rsv_free(void* vec)
{
    if(vec){
        rsv_hd_t *hdr =  RSV_HDR(vec);
        free(hdr);
    }
}

void* rsv_idx(void* vec, size_t idx)
{
    if(!vec){
        FAIL("Cannot get index of a NULL vector\n");
    }

    rsv_hd_t *hdr =  RSV_HDR(vec);
    return (char*)vec + hdr->slt_size * idx;
}


void rsv_qsort(void* vec, int (*compar)(const void *, const void*))
{
    if(!vec){
        FAIL("Cannot sort a NULL vector\n");
    }

    rsv_hd_t *hdr =  RSV_HDR(vec);
    qsort(vec,hdr->obj_count,hdr->slt_size,compar);
}

#endif /* RSVEC_H_ */
