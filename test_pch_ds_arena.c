#include <assert.h>
#include <stdlib.h>
#include <string.h>

static int fail_allocation;

static void *test_allocate(size_t size)
{
    return fail_allocation ? NULL : malloc(size);
}

#define PCH_ARENA_MALLOC test_allocate
#define PCH_ARENA_FREE free
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

static void check_reserve(void)
{
    PchArena arena = { 0 }, moved;
    int *array = NULL, *grown;
    size_t initial_capacity;

    fail_allocation = 1;
    assert(!arrtryres(array, 8, &arena));
    fail_allocation = 0;
    array = arrtryres(array, 8, &arena);
    assert(pch_arr_arena(array) == &arena && !arrlen(array));
    arrpush(array, 42);
    initial_capacity = arrcap(array);
    assert(arrtryres(array, initial_capacity, &arena) == array);

    fail_allocation = 1;
    assert(!arrtryres(array, 65536, &arena));
    assert(!arrtryres(array, (size_t)-1, &arena));
    assert(array[0] == 42 && arrlen(array) == 1 && arrcap(array) == initial_capacity);
    fail_allocation = 0;

    moved = arena;
    arrrebind(array, &moved);
    assert(pch_arr_arena(array) == &moved);
    grown = arrtryres(array, 65536, &moved);
    assert(grown && grown[0] == 42 && arrlen(grown) == 1 && arrcap(grown) >= 65536);
    arrpush(grown, 17);
    assert(grown[1] == 17 && arrlen(grown) == 2);
    pch_arena_destroy(&moved);
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
    check_reserve();
    return 0;
}
