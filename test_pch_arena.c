#include <assert.h>
#include <stdlib.h>

static int fail;
static size_t allocated;
static size_t released;

static void *count_malloc(size_t size)
{
    void *memory = fail ? NULL : malloc(size);
    if (memory) ++allocated;
    return memory;
}

static void count_free(void *memory)
{
    ++released;
    free(memory);
}

#define PCH_ARENA_MALLOC count_malloc
#define PCH_ARENA_FREE count_free
#define PCH_ARENA_IMPLEMENTATION
#include "pch_arena.h"

int main(void)
{
    PchArena arena = { 0 };
    unsigned char *small, *large, *moved;
    long double *aligned;
    size_t blocks;

    small = pch_arena_alloc(&arena, 1);
    assert(small);
    small[0] = 42;
    assert(pch_arena_resize(&arena, small, 1, 33) == small);
    aligned = pch_arena_alloc(&arena, sizeof(*aligned));
    assert(aligned);
    *aligned = 42.0L;
    large = pch_arena_alloc(&arena, 12000);
    assert(large && *aligned == 42.0L && small[0] == 42);
    large[11999] = 17;

    fail = 1;
    assert(!pch_arena_resize(&arena, large, 12000, 60000));
    assert(!pch_arena_alloc(&arena, (size_t)-1));
    assert(large[11999] == 17 && small[0] == 42);
    fail = 0;

    moved = pch_arena_resize(&arena, small, 1, 18000);
    assert(moved && moved != small && moved[0] == 42);
    assert(large[11999] == 17 && small[0] == 42);

    blocks = allocated;
    pch_arena_reset(&arena);
    assert(pch_arena_alloc(&arena, 33) == small);
    assert(pch_arena_alloc(&arena, sizeof(*aligned)) == aligned);
    assert(pch_arena_alloc(&arena, 12000) == large);
    assert(pch_arena_alloc(&arena, 18000) == moved);
    assert(allocated == blocks);
    pch_arena_destroy(&arena);
    assert(allocated == released && !arena.first && !arena.current);
    return 0;
}
