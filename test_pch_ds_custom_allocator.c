#include <assert.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define PCH_REALLOC(A, P, N) \
    ((A) ? arena_realloc((A), (P), (N), (long)PCH_DS_ALLOC_ALIGNMENT) : realloc((P), (N)))
#define PCH_FREE(A, P) ((A) ? (void)0 : free(P))
#define PCH_DS_IMPLEMENTATION
#include "pch_ds.h"

int main(void)
{
    Arena arena = arena_create(8192);
    long double *values = NULL;
    int *heap = NULL;
    int i;

    /* Force the next allocation past an initial pointer-sized boundary. */
    arena_malloc(&arena, sizeof(void *), 1);
    arrinit(values, &arena);
    assert(pch_arr_arena(values) == &arena);
    assert((uintptr_t)PCH_DS_HDR(values) % PCH_DS_ALLOC_ALIGNMENT == 0);
    assert((uintptr_t)values % PCH_DS_ALLOC_ALIGNMENT == 0);

    arrpush(values, 0.5L);
    arena_malloc(&arena, sizeof(void *), 1);
    for (i = 1; i < 32; ++i) {
        arrpush(values, i + 0.5L);
        arrpush(heap, i);
    }
    assert((uintptr_t)PCH_DS_HDR(values) % PCH_DS_ALLOC_ALIGNMENT == 0);
    assert(arrlen(values) == 32 && arrlen(heap) == 31);
    for (i = 0; i < 32; ++i) assert(values[i] == i + 0.5L);
    assert(!pch_arr_arena(heap) && heap[0] == 1 && heap[30] == 31);

    arrfree(heap);
    arrfree(values);
    arena_destroy(&arena);
    return 0;
}
