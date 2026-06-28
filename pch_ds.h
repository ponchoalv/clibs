/* For each over Dynamic Arrays. Example: */
/* ```c

#define PCH_DS_IMPLEMENTATION
#include "pch_ds.h"

 int *xs = NULL;
 int val;
 size_t i;
 arrpush(xs, 69);
 arrpush(xs, 420);

 arrpush(xs, 1337);

 if want to use a custom arena add it to the struct you need to
 define PCH_REALLOC and PCH_FREE (even though you don't want to free
 within the arena):

 for example: (A it will be the arena struct you defined), let's say
 you have

arena_realloc and a free (if you don't care about freeing within the
arena you could pass ((void)0), but should be defined)

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define PCH_REALLOC(A, P, N) arena_realloc(A, P, N, sizeof(void *))
#define PCH_FREE(A, P) ((void)0)

#define PCH_DS_IMPLEMENTATION
#include "pch_ds.h"
sizeof(*(P)),__alignof__(*(P)))

arrinit(xs, &arena);
arrforeach(i, val, xs)
{
    printf("the val at index %zu is %d", i, val);
}


Dynamic Arrays
==============
Initialize the array, is would be needed only if you are using a
custom allocator, like an arena
#define arrinit pch_arrinit

Request more memory for the dynamic array
#define arrmres pch_arrmres

Release the memory of the dynamic array
#define arrfree pch_arrfree

Append a new element to the end of the array
#define arrpush
pch_arrpush

Apend N elements to the end of the array
#define arrpushn pch_arrpushn

Get the last element in the array
#define arrlast pch_arrlast

Get and remove the last element in the array
#define arrpop pch_arrpop

Remove an element unordered
#define arrdelui pch_arrdelui

Insert N blank elements from position i, move the things at the right
#define arrinsn pch_arrinsn

Insert one element at position i.
#define arrinsat pch_arrinsat

Remove an element at i and return in an output parameter the removed
value.
#define arrdel pch_arrdel

Remove An element at i (do not return removed value) #define arrdeli
pch_arrdeli

Remove the first element in the dynamic array, preserving order, is
slow as fuck
#define arrshift pch_arrshift

Replace element at position i, and return the old element as an output
parameter
#define arrreplace pch_arrreplace

Same as `for (i = 0, item = da[i]; i < arrlen(da); i++)`
#define arrforeach pch_arrforeach

Get the current dynamic array capacity
#define arrcap pch_arr_capacity

Change (increase) the array capacity, I think I didn't implement
shrinking it
#define arrsetcap pch_arrsetcap

Get the current count of elements
#define arrlen pch_arr_count

Is empty?
#define arrempty pch_arrempty

Clear the array
#define arrclear pcr_arrclear

Get an slice of the current dynamic array (pch_slice) from start to
finish
#define arrtoslice pch_slice_make

Get same as above but you don't need to specify finish, just start and
will create the slite till the end of the array (pch_slice)
#define arrtoslicer pch_slice_rest

Get a new dynamic array from an slice
#define slicetoarr pch_slice_dup

Loop for each element of the slice
#define sliceforeach pch_slice_foreach

Clone a dynamic array #define arrclone pch_arr_clone

Ring Buffers (backed by a dynamic array)
=======================================
Init, for use with arenas same as dynamic array
#define rinit pch_rinit

Check if it is empty
#define rempty pch_rempty

Enqueue something to the ring
#define renq pch_renq

Dequeue something from the head (beginning of the ring)
#define rdeq pch_rdeq

Get the index of the head (useful for peek)
#define rhead pch_ring_head

Get the index of the tail (useful for peek the last inserted value)
#define rtail pch_ring_tail

Get the len of the ring
#define rlen pch_arr_count

Set the capacity of the ring
#define rsetcap pch_rsetcap

Check if the ring buffer is full (by checking this you can have your
own custom logic to prevent it to keep growing, and do not get any
performance hit)
#define rfull pch_rfull

Clear the Ring
#define rclear pch_rclear

Maps
====
Maps have two Flavors, sh* for maps with strings as keys,
and bh* for everything else as a key.

Same as dynamic array, you will use it with an Arena, otherwise is
optional to init.
#define bhinit pch_bhinit
#define shinit pch_shinit

Is the map empty? #define mempty pch_mempty

Release the map object (by default free, but can be override defining
PCH_FREE with your custom arena implementation) #define shfree
pch_shfree #define bhfree pch_bhfree

Put something into the map will take map, key, and value. #define
bhput pch_bhput #define shput pch_shput


Same as bhput but to use with maps with only keys (aka. set)
#define bhputs pch_bhputs
#define shputs pch_shputs

Get the value at key will default to element at 0 position if key not
found. (thread safe)
#define bhget pch_bhget
#define shget pch_shget

Get pointer to the struct at the key, will return
*struct
{
    TK key;
    TV value;
}

#define bhgetp pch_bhgetp
#define shgetp pch_shgetp

Get pointer to the value for that key -> *TV
#define bhgetvp pch_bhgetvp
#define shgetvp pch_shgetvp

Get the pointer to the key (do not change it, because wont rehash, is
useful for )

#define bhgetkp pch_bhgetkp
#define shgetkp pch_shgetkp

Check if key exists, if exists will give you the index so you can
access with map[i], other wise will return -1 (thread safe)

#define shgeti pch_shgeti
#define bhgeti pch_bhgeti

Remove the thing under the key
#define bhdel pch_bhdel
#define shdel pch_shdel

Get the map length #define mlen pch_mlen


#define bhsetcap pch_bhsetcap
#define shsetcap pch_shsetcap

Clear the map
#define mclear pch_mclear

Check if the last value pulled with bh/shget was successfully (not
thread safe)
#define mfok pch_ok

*/

#ifndef INCLUDE_PCH_DS_H_
#define INCLUDE_PCH_DS_H_
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#ifndef PCH_NO_SHORT_NAMES
/* Dynamic arrays */
#define arrinit pch_arrinit
#define arrmres pch_arrmres
#define arrfree pch_arrfree
#define arrpush pch_arrpush
#define arrpushn pch_arrpushn
#define arrlast pch_arrlast
#define arrpop pch_arrpop
#define arrdelui pch_arrdelui
#define arrinsn pch_arrinsn
#define arrinsat pch_arrinsat
#define arrdel pch_arrdel
#define arrdeli pch_arrdeli
#define arrshift pch_arrshift
#define arrreplace pch_arrreplace
#define arrforeach pch_arrforeach
#define arrcap pch_arr_capacity
#define arrsetcap pch_arrsetcap
#define arrlen pch_arr_count
#define arrempty pch_arrempty
#define arrclear pcr_arrclear
#define arrtoslice pch_slice_make
#define arrtoslicer pch_slice_rest
#define slicetoarr pch_slice_dup
#define sliceforeach pch_slice_foreach
#define arrclone pch_arr_clone
#define arrheappush pch_arrheappush
#define arrheappop pch_arrheappop

/* Ring Buffer */
#define rinit pch_rinit
#define rempty pch_rempty
#define renq pch_renq
#define rdeq pch_rdeq
#define rhead pch_ring_head
#define rtail pch_ring_tail
#define rlen pch_arr_count
#define rsetcap pch_rsetcap
#define rfull pch_rfull
#define rclear pch_rclear
#define rfree pch_rfree

/* Map */
#define bhinit pch_bhinit
#define shinit pch_shinit
#define mempty pch_mempty
#define shfree pch_mfree
#define bhfree pch_mfree
#define bhput pch_bhput
#define shput pch_shput
#define bhputs pch_bhputs
#define shputs pch_shputs
#define bhget pch_bhget
#define shget pch_shget
#define bhgetp pch_bhgetp
#define shgetp pch_shgetp
#define bhgetvp pch_bhgetvp
#define shgetvp pch_shgetvp
#define bhgetkp pch_bhgetkp
#define shgetkp pch_shgetkp
#define shgeti pch_shgeti
#define bhgeti pch_bhgeti
#define bhdel pch_bhdel
#define shdel pch_shdel
#define mlen pch_mlen
#define bhsetcap pch_bhsetcap
#define shsetcap pch_shsetcap
#define mclear pch_mclear
#define mfok pch_ok
#define mfree pch_mfree

#endif /* PCH_NO_SHORT_NAMES */

/* Dynamic Array Implementation took it from https://github.com/tsoding/nob.h */
#ifndef PCH_INIT_CAP
#define PCH_INIT_CAP 1
#endif

/* Must always be a power of 2, me do &MASK instead of modulo for performance */
#ifndef PCH_RING_INIT_CAP
#define PCH_RING_INIT_CAP 1
#endif

/* Must always be a power of 2, me do &MASK instead of modulo for performance */
#ifndef PCH_MAP_INIT_CAP
#define PCH_MAP_INIT_CAP 4
#endif

/* provide your own offset number (prime) on compile time for hash fn */
#ifndef PCH_FNV_OFFSET
#define PCH_FNV_OFFSET 2166136261U
#endif

/* provide your own prime number on compile time for hash fn */
#ifndef PCH_FNV_PRIME
#define PCH_FNV_PRIME 16777619U
#endif

/* Magic constants to detect zero bytes in a word */
#if LONG_BIT == 64
#define ONE_BYTES 0x0101010101010101UL
#define HIGH_BYTES 0x8080808080808080UL
#else
#define ONE_BYTES 0x01010101UL
#define HIGH_BYTES 0x80808080UL
#endif

/* The user can set an specific idx for default values when finding a value failed */
#ifndef PCH_MAP_DEFAULT_IDX
#define PCH_MAP_DEFAULT_IDX 0
#endif

/*
 * Detects if a size_t word contains a null byte (0x00).
 * Source: "Hacker's Delight" / Glibc strlen
 */
#define HAS_ZERO(v) (((v) - ONE_BYTES) & ~(v) & HIGH_BYTES)

#ifndef PCH_MAP_INITIAL_FACTOR
#define PCH_MAP_INITIAL_FACTOR 75
#endif

#define PCH_MAX_IDX ((size_t)-1)

#define PCH_MAP_BINARY_MODE 0
#define PCH_MAP_STRING_MODE 1

#if defined(PCH_REALLOC) && !defined(PCH_FREE) || !defined(PCH_REALLOC) && defined(PCH_FREE)
#error "You must define both PCH_REALLOC and PCH_FREE, or neither."
#endif
#if !defined(PCH_REALLOC) && !defined(PCH_FREE)
#include <stdlib.h>
#define PCH_REALLOC(A, P, N) realloc(P, N)
#define PCH_FREE(A, P) free(P)
#endif

/* STB like header stuff */
typedef struct
{
    void *arena;
    size_t count;
    size_t capacity;
    union {
        struct
        {
            size_t head;
            size_t tail;
        } ring;
        struct
        {
            size_t buckets_offset;
            int factor;
            int current;
        } map;
    } meta;
} pch_ds_hdr;

typedef struct
{
    void *items;
    size_t count;
} pch_slice;

typedef struct
{
    size_t hash;
    int index;
    int tomblestone;
} pch_map_slot;

#define PCH_MAP_SLOTS(da) ((pch_map_slot *)((char *)(da) + PCH_DS_HDR(da)->meta.map.buckets_offset))

#define PCH_MAP_GROW_CHECK(da) (PCH_DS_HDR(da)->count >= ((PCH_DS_HDR(da)->capacity) * (pch_map_factor(da)) / (100)))

#define PCH_DS_HDR(ptr) ((pch_ds_hdr *)(ptr) - 1)
#define PCH_DS_META_RING(ptr) (PCH_DS_HDR(ptr)->meta.ring)

#define pch_arr_count(a) ((a) ? PCH_DS_HDR(a)->count : (0))
#define pch_arrempty(a) (pch_arr_count(a) == 0)
#define pch_arr_capacity(a) ((a) ? PCH_DS_HDR(a)->capacity : PCH_INIT_CAP)
#define pch_ring_capacity(a) ((a) ? PCH_DS_HDR(a)->capacity : PCH_RING_INIT_CAP)
#define pch_arr_arena(a) ((a) ? PCH_DS_HDR(a)->arena : NULL)

#define pch_map_current(a) (PCH_DS_HDR(a)->meta.map.current)
#define pch_map_current_val(a) ((a) ? (PCH_DS_HDR(a)->meta.map.current) : -1)
#define pch_map_factor(a) ((a) ? (PCH_DS_HDR(a)->meta.map.factor) : PCH_MAP_INITIAL_FACTOR)

/* end of STB like header stuff */

/* dynamic array stuff */
#define pch_arrinit(da, arena_ptr) (pch_arrmres((da), (PCH_INIT_CAP), (arena_ptr)))

#define pch_arrpush(da, item)                                                                                          \
    (pch_arrmres((da), pch_arr_count(da) + 1, pch_arr_arena(da)), (da)[PCH_DS_HDR(da)->count++] = (item))

#define pch_arrfree(da) ((void)((da) ? PCH_FREE(pch_arr_arena(da), PCH_DS_HDR(da)) : (void)0), (da) = NULL)

/* Append several items to a dynamic array */
#define pch_arrpushn(da, new_items, new_items_count)                                                                   \
    (pch_arrmres((da), pch_arr_count(da) + (new_items_count)),                                                         \
     memcpy((da) + pch_arr_count(da), (new_items), (new_items_count) * sizeof(*(da))),                                 \
     (PCH_DS_HDR(da)->count += (new_items_count)))

#define pch_arrlast(da) (da)[(assert(pch_arr_count(da) > 0), pch_arr_count(da) - 1)]

#define pch_arrpop(da) (da)[(assert(pch_arr_count(da) > 0), --PCH_DS_HDR(da)->count)]

#define pch_arrdelui(da, idx) (assert((idx) < pch_arr_count(da)), (da)[(idx)] = (da)[--PCH_DS_HDR(da)->count])

#define pch_arrinsn(da, idx, len)                                                                                      \
    (assert((idx) < pch_arr_count(da)), pch_arrmres((da), pch_arr_count(da) + (len), pch_arr_arena(da)),               \
     memmove(&(da)[(idx) + (len)], &(da)[(idx)], sizeof(*(da)) * (pch_arr_count(da) - (idx))),                         \
     PCH_DS_HDR(da)->count += (len))

#define pch_arrinsat(da, idx, item)                                                                                    \
    (pch_arrinsn((da), (idx), 1), (da)[(idx)] = (item),                                                                \
     ((idx) + 1 == pch_arr_count(da) ? (da)[(idx)] : (da)[(idx + 1)]))

#define pch_arrdel(da, idx, item)                                                                                      \
    (assert((idx) < pch_arr_count(da)), ((item) = (da)[(idx)]),                                                        \
     memmove(&(da)[(idx)], &((da)[((idx) + (1))]), sizeof((da)[(idx)]) * (--PCH_DS_HDR(da)->count - (idx))))

#define pch_arrdeli(da, idx)                                                                                           \
    (assert((idx) < pch_arr_count(da)),                                                                                \
     memmove(&(da)[(idx)], &((da)[((idx) + (1))]), sizeof((da)[(idx)]) * (--PCH_DS_HDR(da)->count - (idx))))

#define pch_arrshift(da, item) pch_arrdeli((da), 0, (item))

#define pch_arrmres(da, len, arena_ptr) ((da) = pch_ds_grow((da), sizeof(*(da)), (len), (arena_ptr)))

#define pch_arrsetcap(da, cap) ((da) = pch_ds_grow((da), sizeof(*(da)), (cap), pch_arr_arena(da)))

#define pch_arrreplace(da, idx, item, old_item)                                                                        \
    (assert((idx) < pch_arr_count(da)), ((old_item) = (da)[(idx)]), (da)[(idx)] = (item), old_item)

#define pch_arrforeach(idx, it, da)                                                                                    \
    for ((idx) = 0, (it) = *((da) + (idx)); (idx) < pch_arr_count(da); (idx)++, (it) = *((da) + (idx)))

#define pcr_arrclear(da) (PCH_DS_HDR(da)->count = 0)

#define pch_slice_make(da, start, length)                                                                              \
    pch_slice_from_array((void *)(da), pch_arr_count(da), sizeof(*(da)), (start), (length))

#define pch_slice_rest(da, start)                                                                                      \
    pch_slice_from_array((void *)(da), pch_arr_count(da), sizeof(*(da)), (start), PCH_MAX_IDX)

#define pch_slice_dup(slice, da, arena) ((da) = pch_slice_to_arr((slice), (da), sizeof(*(da)), (arena)))

#define pch_slice_foreach(i, val, slice, type)                                                                         \
    for ((i) = 0; (i) < (slice).count && ((val) = ((type *)(slice).items)[(i)], 1); ++(i))

#define pch_arr_clone(from, to, arena) ((to) = pch_da_clone((from), (to), sizeof(*(to)), (arena)))

/* Comparator: Returns true if A is "better" (smaller f) than B
For example:
  #define LESS_NODE(a, b) ((a).f < (b).f)
*/

// Pushes 'item' into 'da' maintaining heap order.
// LESS is a macro or function: LESS(a, b) returns true if 'a' < 'b' (better priority)
#define pch_arrheappush(da, item, TYPE, LESS)                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        int _i, _p;                                                                                                    \
        TYPE _tmp;                                                                                                     \
        pch_arrpush(da, item);                                                                                         \
        _i = arrlen(da) - 1;                                                                                           \
        while (_i > 0)                                                                                                 \
        {                                                                                                              \
            _p = (_i - 1) / 2;                                                                                         \
            if (LESS((da)[_i], (da)[_p]))                                                                              \
            {                                                                                                          \
                _tmp = (da)[_i];                                                                                       \
                (da)[_i] = (da)[_p];                                                                                   \
                (da)[_p] = _tmp;                                                                                       \
                _i = _p;                                                                                               \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                break;                                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

/* Pops the root (index 0) into 'out_item' and rebalances.
 It does this by returning in out_ptr the value in index 0
 and then removing by putting the last value in the array in index 0
 re-sorting (assuming it is partially sorted) using heap sort.
 Returns 1 if successful, 0 if empty. */
#define pch_arrheappop(da, out_ptr, TYPE, LESS)                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if (pch_arr_count(da) > 0)                                                                                     \
        {                                                                                                              \
            int _i = 0, _l, _r, _smallest, _cnt;                                                                       \
            TYPE _tmp;                                                                                                 \
            *(out_ptr) = (da)[0];                                                                                      \
            pch_arrdelui(da, 0);                                                                                       \
            _cnt = pch_arr_count(da);                                                                                  \
            while (1)                                                                                                  \
            {                                                                                                          \
                _l = 2 * _i + 1;                                                                                       \
                _r = 2 * _i + 2;                                                                                       \
                _smallest = _i;                                                                                        \
                if (_l < _cnt && LESS((da)[_l], (da)[_smallest]))                                                      \
                    _smallest = _l;                                                                                    \
                if (_r < _cnt && LESS((da)[_r], (da)[_smallest]))                                                      \
                    _smallest = _r;                                                                                    \
                if (_smallest != _i)                                                                                   \
                {                                                                                                      \
                    _tmp = (da)[_i];                                                                                   \
                    (da)[_i] = (da)[_smallest];                                                                        \
                    (da)[_smallest] = _tmp;                                                                            \
                    _i = _smallest;                                                                                    \
                }                                                                                                      \
                else                                                                                                   \
                {                                                                                                      \
                    break;                                                                                             \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

/* Ring buffer stuff */

#define pch_ring_head(a) ((a) ? PCH_DS_META_RING(a).head : 0)
#define pch_ring_tail(a) ((a) ? PCH_DS_META_RING(a).tail : 0)
#define pch_ring_mask(a) (pch_arr_capacity(a) - 1)

#define pch_rmres(da, len, arena_ptr) (((da) = pch_ring_grow((da), sizeof(*(da)), (len), (arena_ptr))))

#define pch_rinit(da, arena_ptr) (pch_rmres((da), (PCH_RING_INIT_CAP), (arena_ptr)))

#define pch_rsetcap(da, cap) (pch_rmres((da), (cap), (pch_arr_arena(da))))

#define pch_rempty(da) (pch_arr_count(da) == 0)

#define pch_renq(da, item)                                                                                             \
    (pch_rmres((da), pch_arr_count(da) + 1, pch_arr_arena(da)), (PCH_DS_HDR(da)->count++),                             \
     ((da)[pch_update_ring_tail(da)] = (item)))

#define pch_rdeq(da)                                                                                                   \
    (assert((!pch_rempty(da)) && "Trying to dequeue form an empty queue."), (PCH_DS_HDR(da)->count--),                 \
     ((da)[(pch_update_ring_head(da))]))

#define pch_rfull(da) (pch_arr_count(da) == pch_arr_capacity(da))

#define pch_rclear(da)                                                                                                 \
    ((PCH_DS_HDR(da)->count = 0), (PCH_DS_HDR(da)->meta.ring.tail = 0), (PCH_DS_HDR(da)->meta.ring.head = 0))

#define pch_rfree(da) ((void)((da) ? PCH_FREE(pch_arr_arena(da), PCH_DS_HDR(da)) : (void)0), (da) = NULL)

/* Start Map Stuff */

#define pch_bhinit(da, arena) (pch_mres((da), (PCH_MAP_INIT_CAP), (PCH_MAP_BINARY_MODE), (arena)))

#define pch_shinit(da, arena) (pch_mres((da), (PCH_MAP_INIT_CAP), (PCH_MAP_STRING_MODE), (arena)))

#define pch_bhput(da, k, v)                                                                                            \
    ((pch_mres((da), pch_arr_count(da) + 1, (PCH_MAP_BINARY_MODE), pch_arr_arena(da))),                                \
     (pch_map_put_impl((da), &(k), sizeof(*(da)), sizeof((k)), (PCH_MAP_BINARY_MODE))),                                \
     ((da)[pch_map_current(da)].key = (k)), ((da)[pch_map_current(da)].value = (v)))

#define pch_bhputs(da, k)                                                                                              \
    ((pch_mres((da), pch_arr_count(da) + 1, (PCH_MAP_BINARY_MODE), pch_arr_arena(da))),                                \
     (pch_map_put_impl((da), &(k), sizeof(*(da)), sizeof((k)), (PCH_MAP_BINARY_MODE))),                                \
     ((da)[pch_map_current(da)].key = (k)))

#define pch_shput(da, k, v)                                                                                            \
    (pch_mres((da), pch_arr_count(da) + 1, (PCH_MAP_STRING_MODE), pch_arr_arena(da)),                                  \
     pch_map_put_impl((da), (k), sizeof(*(da)), sizeof((k)), (PCH_MAP_STRING_MODE)),                                   \
     ((da)[pch_map_current(da)].key = (k)), ((da)[pch_map_current(da)].value = (v)))

#define pch_shputs(da, k)                                                                                              \
    (pch_mres((da), pch_arr_count(da) + 1, (PCH_MAP_STRING_MODE), pch_arr_arena(da)),                                  \
     pch_map_put_impl((da), (k), sizeof(*(da)), sizeof((k)), (PCH_MAP_STRING_MODE)),                                   \
     ((da)[pch_map_current(da)].key = (k)))

#define pch_mres(da, len, mode, arena)                                                                                 \
    (!(da) || PCH_MAP_GROW_CHECK(da)                                                                                   \
         ? ((da) = (pch_map_grow((da), sizeof(*(da)), (len), sizeof((da)->key), (mode), arena)), (da))                 \
         : (da))

/* Binary Get: k is passed by address */
#define pch_bhget(da, k)                                                                                               \
    ((da)[pch_map_find_with_default_idx((da), &(k), sizeof(*(da)), sizeof(k), (PCH_MAP_BINARY_MODE))].value)

#define pch_bhgetp(da, k)                                                                                              \
    (&(da)[(pch_map_find_with_default_idx((da), &(k), sizeof(*(da)), sizeof(k), (PCH_MAP_BINARY_MODE)))])

#define pch_bhgetvp(da, k)                                                                                             \
    (&(da)[pch_map_find_with_default_idx((da), (k), sizeof(*(da)), 0, (PCH_MAP_BINARY_MODE))].value)

#define pch_bhgetkp(da, k)                                                                                             \
    (&(da)[(pch_map_find_with_default_idx((da), (k), sizeof(*(da)), 0, (PCH_MAP_BINARY_MODE)))].key)

/* String Get: k is the char* */
#define pch_shget(da, k)                                                                                               \
    (((da)[pch_map_find_with_default_idx((da), (k), sizeof(*(da)), 0, (PCH_MAP_STRING_MODE))].value))

#define pch_shgetp(da, k) (&(da)[pch_map_find_with_default_idx((da), (k), sizeof(*(da)), 0, (PCH_MAP_STRING_MODE))])

#define pch_shgetvp(da, k)                                                                                             \
    (&(da)[pch_map_find_with_default_idx((da), (k), sizeof(*(da)), 0, (PCH_MAP_STRING_MODE))].value)

#define pch_shgetkp(da, k)                                                                                             \
    (&(da)[pch_map_find_with_default_idx((da), (k), sizeof(*(da)), 0, (PCH_MAP_STRING_MODE))].key)

#define pch_ok(da) (pch_map_current(da) != (-1))

#define pch_bhgeti(da, k) ((pch_map_find_i((da), &(k), sizeof(*(da)), sizeof(k), (PCH_MAP_BINARY_MODE))))

#define pch_shgeti(da, k) ((pch_map_find_i((da), (k), sizeof(*(da)), 0, (PCH_MAP_STRING_MODE))))

#define pch_mempty(da) (pch_arr_count(da) == 0)

#define pch_mlen(da) (pch_arr_count(da))

#define pch_bhsetcap(da, len)                                                                                          \
    (pch_mres((da), ((pch_arr_count(da) > (len)) ? pch_arr_count(da) : (len)), (PCH_MAP_BINARY_MODE),                  \
              (pch_arr_arena(da))))

#define pch_shsetcap(da, len)                                                                                          \
    (pch_mres((da), ((pch_arr_count(da) > (len)) ? pch_arr_count(da) : (len)), (PCH_MAP_STRING_MODE),                  \
              (pch_arr_arena(da))))

#define pch_mclear(da) ((da) = (pch_map_clear((da))))

#define pch_bhdel(da, k) (pch_map_del((da), sizeof(*(da)), &(k), sizeof((k)), (PCH_MAP_BINARY_MODE)))

#define pch_shdel(da, k) (pch_map_del((da), sizeof(*(da)), (k), sizeof((k)), (PCH_MAP_STRING_MODE)))

#define pch_mfree(da) ((void)((da) ? PCH_FREE(pch_arr_arena(da), PCH_DS_HDR(da)) : (void)0), (da) = NULL)

static size_t pch_next_pow2(size_t n, size_t init);
void *pch_ds_grow(void *da, size_t item_size, size_t needed, void *arena_ptr);
void *pch_ring_grow(void *ptr, size_t item_size, size_t needed, void *arena_ptr);
void *pch_map_clear(void *da);
int pch_map_del(void *da, size_t item_size, const void *key, size_t keysize, int mode);
size_t pch_update_ring_head(void *ptr);
size_t pch_update_ring_tail(void *ptr);
pch_slice pch_slice_from_array(void *ptr, size_t total_count, size_t item_size, size_t start, size_t requested_len);
void *pch_slice_to_arr(pch_slice s, void *new_da, size_t item_size, void *arena);
void *pch_da_clone(void *from, void *to, size_t item_size, void *arena);
void *pch_map_grow(void *ptr, size_t item_size, size_t needed, size_t keysize, int mode, void *arena);
int pch_map_find_i(void *da, const void *key, size_t item_size, size_t keysize, int mode);
int pch_map_find_with_default_idx(void *da, const void *key, size_t item_size, size_t keysize, int mode);
void pch_map_put_impl(void *da, const void *key, size_t item_size, size_t keysize, int mode);

#endif /* INCLUDE_PCH_DS_H_ */

#ifdef PCH_DS_IMPLEMENTATION

static size_t pch_next_pow2(size_t n, size_t init)
{
    if (n == 0)
        return init;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if (sizeof(size_t) > 4)
        n |= n >> 32;
    n++;
    return n;
}

void *pch_ds_grow(void *da, size_t item_size, size_t needed, void *arena_ptr)
{
    size_t cap = pch_arr_capacity(da);
    size_t total_size;
    void *h;
    pch_ds_hdr *hdr;

    if (da != NULL && needed <= cap)
        return da;

    cap = pch_next_pow2(needed, PCH_INIT_CAP);

    /* Calculate total size: Header + Data */
    total_size = sizeof(pch_ds_hdr) + (cap * item_size);

    h = PCH_REALLOC(arena_ptr, da ? PCH_DS_HDR(da) : NULL, total_size);
    assert(h != NULL && "No more ram for you");

    hdr = (pch_ds_hdr *)h;
    hdr->capacity = cap;
    if (!da)
    {
        hdr->count = 0;
        hdr->arena = arena_ptr;
    }

    return (void *)(hdr + 1);
}

pch_slice pch_slice_from_array(void *da, size_t total_count, size_t item_size, size_t start, size_t requested_len)
{
    pch_slice s;

    if (start >= total_count)
    {
        s.items = NULL;
        s.count = 0;
        return s;
    }

    if (start + requested_len > total_count || requested_len == PCH_MAX_IDX)
    {
        s.count = total_count - start;
    }
    else
    {
        s.count = requested_len;
    }

    s.items = (void *)((char *)da + (start * item_size));

    return s;
}

void *pch_slice_to_arr(pch_slice s, void *new_da, size_t item_size, void *arena)
{
    /* Create a new dynamic array with the required capacity */
    new_da = pch_ds_grow(new_da, item_size, s.count, arena);
    memcpy(new_da, s.items, s.count * item_size);
    PCH_DS_HDR(new_da)->count = s.count;
    return new_da;
}

void *pch_da_clone(void *from, void *to, size_t item_size, void *arena)
{
    to = pch_ds_grow(to, item_size, pch_arr_count(from), arena);
    memcpy(to, PCH_DS_HDR(from), sizeof(pch_ds_hdr) + pch_arr_count(from) * item_size);

    to = (pch_ds_hdr *)to + 1;

    return to;
}

void *pch_ring_grow(void *da, size_t item_size, size_t needed, void *arena_ptr)
{
    pch_ds_hdr *old_hdr;
    pch_ds_hdr *new_hdr;
    void *new_raw;
    void *new_ptr;
    size_t old_cap;
    size_t new_cap;
    size_t head;
    size_t tail;
    size_t total_bytes;

    old_hdr = (da != NULL) ? PCH_DS_HDR(da) : NULL;
    old_cap = pch_arr_capacity(da);

    if (needed <= old_cap && da != NULL)
    {
        return da;
    }

    /* 1. Calculate new power-of-two capacity */
    new_cap = (needed < PCH_RING_INIT_CAP) ? PCH_RING_INIT_CAP : needed;
    new_cap = pch_next_pow2(new_cap, PCH_RING_INIT_CAP);

    /* 2. Grow the block in-place if possible */
    total_bytes = sizeof(pch_ds_hdr) + (new_cap * item_size);
    new_raw = PCH_REALLOC(arena_ptr, old_hdr, total_bytes);
    assert(new_raw != NULL && "Allocation failed");

    new_hdr = (pch_ds_hdr *)new_raw;
    new_ptr = (void *)((char *)new_hdr + sizeof(pch_ds_hdr));
    new_hdr->capacity = new_cap;

    if (old_hdr == NULL)
    {
        new_hdr->arena = arena_ptr;
        new_hdr->count = 0;
        new_hdr->meta.ring.head = 0;
        new_hdr->meta.ring.tail = 0;
    }
    else
    {
        head = new_hdr->meta.ring.head;
        tail = new_hdr->meta.ring.tail;

        /* 3. Handle the Wrapped State
           If tail <= head and the buffer is not empty (count > 0),
           the data is logically wrapped around the physical end.
        */
        if (new_hdr->count > 0 && tail <= head)
        {
            /* We move the segment from [head ... old_cap] to the
               new end of the buffer [new_cap - (old_cap - head) ... new_cap]
            */
            size_t items_to_move = old_cap - head;
            size_t new_head = new_cap - items_to_move;

            void *src = (char *)new_ptr + (head * item_size);
            void *dst = (char *)new_ptr + (new_head * item_size);

            memmove(dst, src, items_to_move * item_size);

            new_hdr->meta.ring.head = new_head;
        }
    }

    return new_ptr;
}

size_t pch_update_ring_head(void *da)
{
    size_t old_head = PCH_DS_HDR(da)->meta.ring.head;
    PCH_DS_HDR(da)->meta.ring.head = ((pch_ring_head(da) + 1) & (pch_ring_mask(da)));
    return old_head;
}

size_t pch_update_ring_tail(void *da)
{
    size_t old_tail = pch_ring_tail(da);
    PCH_DS_HDR(da)->meta.ring.tail = ((old_tail + 1) & pch_ring_mask(da));
    return old_tail;
}

/* Map Stuff starts here */

// static size_t pch_hash_string(const char *str)
// {
//     size_t hash = PCH_FNV_OFFSET;

//     while (*str)
//     {
//         hash ^= (size_t)(*str++);
//         hash *= PCH_FNV_PRIME;
//     }

//     return (hash == 0) ? 1 : hash;
// }

static size_t pch_hash_string_fast(const char *str)
{
    const size_t *word_ptr;
    size_t hash = PCH_FNV_OFFSET;
    size_t word;

    /* 1. Handle unaligned preamble (optional but safer)
     * If you want absolute raw speed and are on x64, you can skip this
     * and cast directly, provided you don't crash on page boundaries.
     * For safety, usually we consume bytes until aligned.
     */
    while (((size_t)str) & (sizeof(size_t) - 1))
    {
        if (!*str)
            return (hash == 0) ? 1 : hash;
        hash ^= (size_t)(*str++);
        hash *= PCH_FNV_PRIME;
    }

    /* 2. Main Loop: Process 4 or 8 bytes at a time */
    word_ptr = (const size_t *)str;

    while (1)
    {
        word = *word_ptr; // Fetch next word

        /* Check if this word contains a \0 terminator */
        if (HAS_ZERO(word))
        {
            /* Found the end! Break to handle the final bytes */
            break;
        }

        /* No null terminator, hash the whole word */
        hash ^= word;
        hash *= PCH_FNV_PRIME;
        word_ptr++;
    }

    /* 3. Handle the final bytes (tail) */
    /* We know the null terminator is inside 'word', need to find where */
    str = (const char *)word_ptr;
    while (*str)
    {
        hash ^= (size_t)(*str++);
        hash *= PCH_FNV_PRIME;
    }

    return (hash == 0) ? 1 : hash;
}

// static size_t pch_hash_bytes(const void *da, size_t len)
// {
//     const unsigned char *p = (const unsigned char *)da;
//     size_t hash = PCH_FNV_OFFSET;
//     size_t i;

//     for (i = 0; i < len; ++i)
//     {
//         hash ^= (size_t)p[i];
//         hash *= PCH_FNV_PRIME;
//     }
//     return (hash == 0) ? 1 : hash;
// }

/* * A very fast word-at-a-time hash (similar to FxHash).
 * Much faster than FNV because it consumes sizeof(size_t) bytes per step.
 */
static size_t pch_hash_bytes_fast(const void *da, size_t len)
{
    const size_t *words = (const size_t *)da;
    const unsigned char *p;
    size_t hash = PCH_FNV_OFFSET;
    size_t word_count = len / sizeof(size_t);
    size_t i;

    /* 1. Process standard word-sized chunks */
    for (i = 0; i < word_count; ++i)
    {
        /* * Combine the hash with the whole word.
         * Using a rotate here (if available) is even better,
         * but standard XOR-MUL is very fast.
         */
        hash ^= words[i];
        hash *= PCH_FNV_PRIME;
    }

    /* 2. Handle remaining bytes (the "tail") */
    p = (const unsigned char *)(words + word_count);
    i = len % sizeof(size_t);

    while (i--)
    {
        hash ^= (size_t)(*p++);
        hash *= PCH_FNV_PRIME;
    }

    return (hash == 0) ? 1 : hash;
}

static int pch_is_key_equal(void *da, const void *key, size_t item_size, size_t keysize, int mode, int index)
{
    void *entry = (char *)da + (item_size * index);

    if (mode >= PCH_MAP_STRING_MODE)
    {
        const char *search_key = (const char *)key;
        const char *entry_key = *(const char **)entry;
        return (entry_key && search_key) ? 0 == (strcmp(search_key, entry_key)) : 0;
    }
    else
    {
        return 0 == memcmp(key, entry, keysize);
    }
}

static size_t pch_hash_fn(const void *ptr, size_t keysize, int mode)
{
    if (mode >= PCH_MAP_STRING_MODE)
        return pch_hash_string_fast(*(char **)ptr);
    else
        return pch_hash_bytes_fast(ptr, keysize);
}

static void pch_map_rehash(void *da, size_t item_size, size_t keysize, int mode)
{
    pch_ds_hdr *da_h;
    pch_map_slot *slots;
    size_t new_mask, i, h, idx;
    void *entry;

    da_h = PCH_DS_HDR(da);

    /* Sparse array lives at the end: Header + Dense[Capacity] */
    slots = PCH_MAP_SLOTS(da);
    new_mask = (pch_arr_capacity(da) * 2) - 1;

    /* 4. Initialize hashes to 0 */
    memset(slots, 0, (pch_arr_capacity(da) * 2) * sizeof(pch_map_slot));

    // printf("count is %zu\n", da_h->count);
    // printf("capacity is %zu\n", da_h->capacity);
    for (i = 0; i < da_h->count; ++i)
    {
        entry = (char *)da + (i * item_size);

        /* 1. Calculate hash of the key (Assumed to be at start of struct) */
        h = pch_hash_fn(entry, keysize, mode);
        idx = h & new_mask;

        /* 2. Linear probe for an empty slot in the new sparse array */
        while (slots[idx].hash != 0)
        {
            idx = (idx + 1) & new_mask;
        }

        /* 3. Link sparse slot to dense index */
        slots[idx].hash = h;
        slots[idx].index = (int)i;
        slots[idx].tomblestone = 0;
    }
}

void *pch_map_grow(void *da, size_t item_size, size_t needed, size_t keysize, int mode, void *arena)
{
    pch_ds_hdr *current_h = NULL;
    size_t new_cap;
    size_t num_buckets;
    size_t dense_size;
    size_t total_size;
    void *new_da;
    void *h;

    new_cap = (needed < PCH_MAP_INIT_CAP) ? PCH_MAP_INIT_CAP : needed;
    new_cap = pch_next_pow2(new_cap, PCH_MAP_INIT_CAP);

    if ((da != NULL) && new_cap <= pch_arr_capacity(da))
    {
        return da;
    }

    num_buckets = new_cap * 2;

    /* 2. Calculate Total Size: Header + Dense + Sparse */
    dense_size = new_cap * item_size;
    total_size = sizeof(pch_ds_hdr) + dense_size + (num_buckets * sizeof(pch_map_slot));

    /* 3. Allocation */
    h = PCH_REALLOC(arena, (da) ? PCH_DS_HDR(da) : NULL, total_size);
    assert(h != NULL && "No more ram for you");

    current_h = (pch_ds_hdr *)h;
    new_da = (void *)(current_h + 1);

    current_h->capacity = new_cap;
    current_h->meta.map.buckets_offset = dense_size;
    current_h->meta.map.current = -1;

    /* 5. Rehash if old data exists */
    if (da)
    {
        pch_map_rehash(new_da, item_size, keysize, mode);
    }
    else
    {
        current_h->arena = arena;
        current_h->count = 0;
        current_h->meta.map.factor = PCH_MAP_INITIAL_FACTOR;
        current_h->meta.map.current = -1;
    }

    return new_da;
}

int pch_map_find_i(void *da, const void *key, size_t item_size, size_t keysize, int mode)
{
    pch_ds_hdr *h;
    pch_map_slot *slots;
    size_t hash, mask, idx, start;

    if (!da)
    {
        pch_map_current(da) = -1;
        return -1;
    }

    h = PCH_DS_HDR(da);
    slots = PCH_MAP_SLOTS(da);
    mask = (h->capacity * 2) - 1;

    hash = (mode >= PCH_MAP_STRING_MODE) ? pch_hash_string_fast((const char *)key) : pch_hash_bytes_fast(key, keysize);
    idx = hash & mask;
    start = idx;
    while (slots[idx].hash != 0 || slots[idx].tomblestone != 0)
    {
        if (slots[idx].hash == hash)
        {
            if (pch_is_key_equal(da, key, item_size, keysize, mode, slots[idx].index))
            {
                // printf("found at index %d\n", slots[idx].index);
                pch_map_current(da) = slots[idx].index;
                return slots[idx].index;
            }
        }
        idx = (idx + 1) & mask;

        if (idx == start)
        {
            break;
        }
    }
    pch_map_current(da) = -1;
    return -1;
}

int pch_map_find_with_default_idx(void *da, const void *key, size_t item_size, size_t keysize, int mode)
{
    int found = pch_map_find_i(da, key, item_size, keysize, mode);

    return found > -1 ? found : PCH_MAP_DEFAULT_IDX;
}

void pch_map_put_impl(void *da, const void *key, size_t item_size, size_t keysize, int mode)
{
    pch_ds_hdr *h;
    pch_map_slot *slots;
    size_t hash, mask, idx, start;

    h = PCH_DS_HDR(da);
    slots = PCH_MAP_SLOTS(da);
    mask = (h->capacity * 2) - 1;

    hash = (mode >= PCH_MAP_STRING_MODE) ? pch_hash_string_fast((const char *)key) : pch_hash_bytes_fast(key, keysize);
    idx = hash & mask;
    start = idx;

    /* 2. The Single-Pass Probe */
    while (slots[idx].hash != 0 || slots[idx].tomblestone != 0)
    {
        /* Check if key already exists to update it */
        if (slots[idx].hash == hash)
        {
            if (pch_is_key_equal(da, key, item_size, keysize, mode, slots[idx].index))
            {
                pch_map_current(da) = slots[idx].index;
                return;
            }
        }
        idx = (idx + 1) & mask;

        if (idx == start)
        {
            break;
        }
    }

    /* 3. If we are here, we found an empty slot (slots[idx].hash == 0) */
    slots[idx].hash = hash;
    slots[idx].index = (int)h->count;
    slots[idx].tomblestone = 0;
    pch_map_current(da) = (int)h->count;
    h->count++;
    return;
}

void *pch_map_clear(void *da)
{
    pch_map_slot *slots;

    if (!da)
    {
        return da;
    }

    slots = PCH_MAP_SLOTS(da);
    PCH_DS_HDR(da)->count = 0;
    memset(slots, 0, PCH_DS_HDR(da)->capacity * 2 * sizeof(pch_map_slot));

    return da;
}

int pch_map_del(void *da, size_t item_size, const void *key, size_t keysize, int mode)
{
    pch_ds_hdr *h;
    pch_map_slot *slots;
    size_t hash, mask, idx, start;
    int final_index = (int)pch_arr_count(da) - 1;
    int i = -1;

    h = PCH_DS_HDR(da);
    slots = PCH_MAP_SLOTS(da);
    mask = (h->capacity * 2) - 1;

    hash = (mode >= PCH_MAP_STRING_MODE) ? pch_hash_string_fast((const char *)key) : pch_hash_bytes_fast(key, keysize);
    idx = hash & mask;
    start = idx;
    while (slots[idx].hash != 0 || slots[idx].tomblestone != 0)
    {
        if (slots[idx].hash == hash)
        {
            if (pch_is_key_equal(da, key, item_size, keysize, mode, slots[idx].index))
            {
                i = slots[idx].index;
                slots[idx].index = -1;
                slots[idx].hash = 0;
                slots[idx].tomblestone = 1;
                break;
            }
        }
        idx = (idx + 1) & mask;

        if (idx == start)
            break;
    }

    if (i == -1)
    {
        return 0;
    }
    else if (i == final_index)
    {
        h->count--;
        return 1;
    }
    else
    {
        memmove((char *)da + item_size * i, (char *)da + item_size * final_index, item_size);
        hash = (mode >= PCH_MAP_STRING_MODE) ? pch_hash_string_fast(*(char **)((char *)da + item_size * i))
                                             : pch_hash_bytes_fast((char *)da + item_size * i, keysize);
        idx = hash & mask;

        while (slots[idx].hash != 0 || slots[idx].tomblestone != 0)
        {
            if (slots[idx].hash == hash)
            {
                if ((mode >= PCH_MAP_STRING_MODE)
                        ? pch_is_key_equal(da, *(char **)(char *)da + item_size * i, item_size, keysize, mode,
                                           slots[idx].index)
                        : pch_is_key_equal(da, (char *)da + item_size * i, item_size, keysize, mode, slots[idx].index))
                {
                    /* i = slots[idx].index; */
                    slots[idx].index = i;
                    /* slots[idx].hash = hash; */
                    h->count--;
                    return 1;
                }
            }
            idx = (idx + 1) & mask;
        }
    }

    printf("we reach unreachable\n");
    /* Shouldn't be reach */
    return 0;
}

#endif /* PCH_DS_IMPLEMENTATION */
