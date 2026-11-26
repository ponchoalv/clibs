#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static int fail_arena_malloc;

static void *test_malloc(size_t size)
{
    return fail_arena_malloc ? NULL : malloc(size);
}

#define PCH_ARENA_MALLOC test_malloc
#define PCH_ARENA_FREE free
#define PCH_ARENA_IMPLEMENTATION
#define PCH_DS_IMPLEMENTATION
#define PCH_DS_USE_ARENA
#include "pch_ds.h"

typedef struct {
    void *arena;
    size_t count, capacity;
    union {
        long double alignment;
        struct { size_t head, tail; } ring;
        struct { size_t buckets_offset; int factor, current; } map;
    } meta;
} HeaderWithoutPriority;

typedef struct {
    int priority, id;
} Job;

typedef struct {
    int priority;
    unsigned char bytes[4096];
} LargeJob;

static int compare_priority(int a, int b)
{
    return (a > b) - (a < b);
}

static int low_priority_first(const void *a, const void *b)
{
    const Job *left = a, *right = b;
    int order = compare_priority(left->priority, right->priority);
    return order ? order : compare_priority(left->id, right->id);
}

static int high_priority_first(const void *a, const void *b)
{
    return low_priority_first(b, a);
}

static int compare_large_jobs(const void *a, const void *b)
{
    const LargeJob *left = a, *right = b;
    return compare_priority(left->priority, right->priority);
}

static void check_order(PchArena *arena, int descending)
{
    Job *queue = NULL, *saved;
    Job item = { 0, 0 };
    Job previous, removed;
    pch_pq_cmp cmp = descending ? high_priority_first : low_priority_first;
    size_t i, capacity;

    assert(!pqcap(queue) && pqempty(queue) && !pqpeek(queue));
    assert(!pqpush(queue, item) && !pqpop(queue, &item) && !pqfix(queue, 0));
    assert(!pqinit(queue, arena, NULL));
    assert(pqinit(queue, arena, cmp) && pch_arr_arena(queue) == arena);
    assert(pqreserve(queue, 47) && pqcap(queue) >= 47);
    for (i = 0; i < 1024; ++i) {
        item.priority = (int)(i % 97);
        item.id = (int)i;
        assert(pqpush(queue, item));
    }
    assert(pqlen(queue) == 1024 && PCH_DS_HDR(queue)->meta.priority.cmp == cmp);

    item = queue[31];
    assert(pqremove(queue, 31, &removed));
    assert(item.id == removed.id && item.priority == removed.priority);
    item = queue[pqlen(queue) - 1];
    assert(pqremove(queue, pqlen(queue) - 1, &removed) && item.id == removed.id);
    assert(!pqremove(queue, pqlen(queue), &removed));

    queue[45].priority = descending ? INT_MAX : INT_MIN;
    item = queue[45];
    assert(pqfix(queue, 45) && pqpeek(queue)->id == item.id);
    queue[0].priority = descending ? INT_MIN : INT_MAX;
    assert(pqfix(queue, 0) && pqpeek(queue)->id != item.id);

    saved = queue;
    capacity = pqcap(queue);
    assert(pqreserve(queue, (size_t)-1) == saved && pqcap(queue) == capacity && pqlen(queue) == 1022);
    assert(pqpop(queue, &previous));
    for (i = 1; i < 1022; ++i) {
        assert(pqpop(queue, &item));
        assert(cmp(&previous, &item) <= 0);
        previous = item;
    }
    assert(!pqpop(queue, NULL) && !pqpeek(queue) && pqempty(queue));
    assert(pqpush(queue, item) && pqpop(queue, NULL));
    assert(pqpush(queue, item));
    pqclear(queue);
    assert(pqempty(queue) && pqcap(queue) == capacity);
    pqfree(queue);
    pqclear(queue);
    assert(!queue);
}

static void check_failed_growth(void)
{
    PchArena arena = { 0 };
    LargeJob *queue = NULL, *saved;
    LargeJob item, out;
    size_t i, initial_capacity;

    memset(&item, 42, sizeof(item));
    fail_arena_malloc = 1;
    assert(!pqinit(queue, &arena, compare_large_jobs) && !queue);
    fail_arena_malloc = 0;
    assert(pqinit(queue, &arena, compare_large_jobs));
    initial_capacity = pqcap(queue);
    for (i = 0; i < initial_capacity; ++i) {
        item.priority = (int)i + 10;
        assert(pqpush(queue, item));
    }

    saved = queue;
    fail_arena_malloc = 1;
    item.priority = 1;
    assert(!pqpush(queue, item) && queue == saved && pqlen(queue) == initial_capacity);
    assert(pqpeek(queue)->priority == 10 && pqpeek(queue)->bytes[4095] == 42);
    fail_arena_malloc = 0;
    memset(item.bytes, 99, sizeof(item.bytes));
    assert(pqpush(queue, item) && pqpop(queue, &out));
    assert(out.priority == 1 && out.bytes[4095] == 99);
    assert(pqpop(queue, &out) && out.priority == 10 && out.bytes[4095] == 42);
    pqfree(queue);
    pch_arena_destroy(&arena);
}

int main(void)
{
    PchArena arena = { 0 };
    assert(sizeof(pch_ds_hdr) == sizeof(HeaderWithoutPriority));
    check_order(NULL, 1);
    check_order(&arena, 0);
    pch_arena_reset(&arena);
    check_order(&arena, 1);
    pch_arena_destroy(&arena);
    check_failed_growth();
    return 0;
}
