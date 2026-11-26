#ifndef PCH_ARENA_H
#define PCH_ARENA_H

#include <stddef.h>

typedef struct PchArenaBlock PchArenaBlock;

/* Zero-initialize before use. The value owns its blocks and can be moved
 * between owners. Returned pointers live until reset or destroy. */
typedef struct {
    PchArenaBlock *first;
    PchArenaBlock *current;
    void *last;
    size_t last_offset;
} PchArena;

void *pch_arena_alloc(PchArena *arena, size_t size);

/* Pass a live allocation from this arena and the number of bytes to preserve.
 * Growth may return a new address. Failure leaves the old allocation intact. */
void *pch_arena_resize(PchArena *arena, void *memory, size_t used, size_t size);

/* Reset keeps blocks for reuse. Destroy releases all blocks. */
void pch_arena_reset(PchArena *arena);
void pch_arena_destroy(PchArena *arena);

#endif

#ifdef PCH_ARENA_IMPLEMENTATION
#ifndef PCH_ARENA_IMPLEMENTATION_INCLUDED
#define PCH_ARENA_IMPLEMENTATION_INCLUDED

#include <stdlib.h>
#include <string.h>

#if defined(PCH_ARENA_MALLOC) != defined(PCH_ARENA_FREE)
#error "Define both PCH_ARENA_MALLOC and PCH_ARENA_FREE, or neither."
#endif
#ifndef PCH_ARENA_MALLOC
#define PCH_ARENA_MALLOC malloc
#define PCH_ARENA_FREE free
#endif

typedef union {
    long integer;
    size_t size;
    long double real;
    void *object;
    void (*function)(void);
} PchArenaAlignment;

struct PchArenaBlock {
    PchArenaBlock *next;
    size_t capacity;
    size_t used;
    /* Align the payload reached through block + 1. */
    PchArenaAlignment alignment;
};

static void *pch_arena_take(PchArena *arena, PchArenaBlock *block, size_t offset, size_t size)
{
    unsigned char *payload = (unsigned char *)(block + 1);
    block->used = offset + size;
    arena->current = block;
    arena->last_offset = offset;
    arena->last = payload + offset;
    return arena->last;
}

void *pch_arena_alloc(PchArena *arena, size_t size)
{
    PchArenaBlock *block = arena->current;
    PchArenaBlock *last = NULL;
    size_t capacity = 4096;
    size_t limit = (size_t)-1 - sizeof(*block);
    size_t alignment = sizeof(PchArenaAlignment);
    size_t remaining, padding, growth;

    if (!size) size = 1;
    if (size > limit) return NULL;
    while (block) {
        remaining = block->capacity - block->used;
        padding = block->used % alignment;
        if (padding) padding = alignment - padding;
        if (padding <= remaining && size <= remaining - padding)
            return pch_arena_take(arena, block, block->used + padding, size);
        last = block;
        block = block->next;
    }

    if (capacity > limit) capacity = limit;
    if (last && last->capacity >= capacity) {
        growth = last->capacity / 2 + 1;
        capacity = growth > limit - last->capacity ? limit : last->capacity + growth;
    }
    if (capacity < size) capacity = size;
    block = PCH_ARENA_MALLOC(sizeof(*block) + capacity);
    if (!block) return NULL;
    block->next = NULL;
    block->capacity = capacity;
    if (last) last->next = block;
    else arena->first = block;
    return pch_arena_take(arena, block, 0, size);
}

void *pch_arena_resize(PchArena *arena, void *memory, size_t used, size_t size)
{
    void *storage;

    if (!memory) return pch_arena_alloc(arena, size);
    if (size <= used) return memory;
    if (arena->current && memory == arena->last &&
        size <= arena->current->capacity - arena->last_offset) {
        arena->current->used = arena->last_offset + size;
        return memory;
    }
    storage = pch_arena_alloc(arena, size);
    if (storage && used) memcpy(storage, memory, used);
    return storage;
}

void pch_arena_reset(PchArena *arena)
{
    PchArenaBlock *block;
    for (block = arena->first; block; block = block->next) block->used = 0;
    arena->current = arena->first;
    arena->last = NULL;
    arena->last_offset = 0;
}

void pch_arena_destroy(PchArena *arena)
{
    PchArena empty = { 0 };
    PchArenaBlock *block = arena->first;
    while (block) {
        PchArenaBlock *next = block->next;
        PCH_ARENA_FREE(block);
        block = next;
    }
    *arena = empty;
}

#endif
#endif
