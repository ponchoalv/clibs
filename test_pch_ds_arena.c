#include <assert.h>
#include <string.h>

#define PCH_ARENA_IMPLEMENTATION
#define PCH_DS_IMPLEMENTATION
#define PCH_DS_USE_ARENA
#include "pch_ds.h"

typedef struct {
    int key, value;
} IntPair;

typedef struct {
    const char *key;
    int value;
} StringPair;

static void make_key(char *name, int number)
{
    int i;
    memcpy(name, "key-0000", sizeof("key-0000"));
    for (i = 7; i >= 4; --i) {
        name[i] = (char)('0' + number % 10);
        number /= 10;
    }
}

static void check_collections(PchArena *arena)
{
    long double *numbers = NULL;
    int *queue = NULL, *heap = NULL;
    IntPair *integers = NULL;
    StringPair *strings = NULL;
    const char *key;
    char *storage;
    int i;

    arrinit(numbers, arena);
    rinit(queue, arena);
    bhinit(integers, arena);
    shinit(strings, arena);
    assert(pch_arr_arena(numbers) == arena && pch_arr_arena(queue) == arena &&
           pch_arr_arena(integers) == arena && pch_arr_arena(strings) == arena);

    for (i = 0; i < 1024; ++i) {
        arrpush(numbers, (long double)i + 0.5L);
        bhput(integers, i, i + 42);
        storage = pch_arena_alloc(arena, sizeof("key-0000"));
        assert(storage);
        make_key(storage, i);
        key = storage;
        shput(strings, key, i + 7);
        if (i < 128) renq(queue, i);
    }
    for (i = 0; i < 90; ++i) assert(rdeq(queue) == i);
    for (i = 128; i < 1024; ++i) renq(queue, i);
    for (i = 90; i < 1024; ++i) assert(rdeq(queue) == i);
    assert(rempty(queue) && arrlen(numbers) == 1024 && mlen(integers) == 1024 && mlen(strings) == 1024);

    for (i = 0; i < 1024; ++i) {
        char name[sizeof("key-0000")];
        make_key(name, i);
        assert(numbers[i] == (long double)i + 0.5L);
        assert(bhgeti(integers, i) >= 0 && bhget(integers, i) == i + 42);
        assert(shgeti(strings, name) >= 0 && shget(strings, name) == i + 7);
    }
    for (i = 0; i < 1024; i += 2) {
        char key_to_delete[sizeof("key-0000")];
        make_key(key_to_delete, i);
        assert(bhdel(integers, i) && bhgeti(integers, i) < 0);
        assert(shdel(strings, key_to_delete) && shgeti(strings, key_to_delete) < 0);
    }
    for (i = 1; i < 1024; i += 2) {
        char name[sizeof("key-0000")];
        make_key(name, i);
        assert(bhgeti(integers, i) >= 0 && bhget(integers, i) == i + 42);
        assert(shgeti(strings, name) >= 0 && shget(strings, name) == i + 7);
    }
    for (i = 0; i < 16; ++i) arrpush(heap, i);
    assert(!pch_arr_arena(heap) && heap[15] == 15);

    arrfree(numbers);
    rfree(queue);
    mfree(integers);
    mfree(strings);
    arrfree(heap);
    assert(!numbers && !queue && !integers && !strings && !heap);
}

int main(void)
{
    PchArena arena = { 0 };
    PchArenaBlock *blocks;
    check_collections(&arena);
    blocks = arena.first;
    pch_arena_reset(&arena);
    assert(arena.first == blocks);
    check_collections(&arena);
    pch_arena_destroy(&arena);
    assert(!arena.first);
    return 0;
}
