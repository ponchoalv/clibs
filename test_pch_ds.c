/* Run from clibs:
 * rtk proxy clang -std=gnu89 -Wall -Wextra test_pch_ds.c -o /tmp/clibs-test-pch-ds
 * rtk proxy /tmp/clibs-test-pch-ds
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *dirty_realloc(void *ptr, size_t size)
{
    int fresh = ptr == NULL;
    void *result = realloc(ptr, size);
    assert(result != NULL);
    if (fresh)
        memset(result, 0xa5, size);
    return result;
}

#define PCH_REALLOC(A, P, N) dirty_realloc(P, N)
#define PCH_FREE(A, P) free(P)
#define PCH_DS_IMPLEMENTATION
#include "pch_ds.h"

typedef struct
{
    int key;
    int value;
} Entry;

int main(void)
{
    Entry *map = NULL;
    int first = 0, second = 8, fixed = 1000, moving = 0, next;

    /* Fresh maps must work with memory that was not already zeroed. */
    bhinit(map, NULL);
    bhput(map, first, 10);
    bhput(map, second, 20);
    assert(mlen(map) == 2);
    assert(bhgeti(map, first) >= 0 && bhget(map, first) == 10);
    assert(bhgeti(map, second) >= 0 && bhget(map, second) == 20);

    /* These keys collide in the default eight buckets. Search beyond deletion. */
    assert(bhdel(map, first));
    bhput(map, second, 30);
    assert(mlen(map) == 1);
    assert(bhgeti(map, second) >= 0 && bhget(map, second) == 30);

    /* Repeated movement must not exhaust tombstones and lose a live entry. */
    mclear(map);
    bhput(map, fixed, 40);
    bhput(map, moving, 50);
    for (next = 1; next <= 64; next++)
    {
        assert(bhdel(map, moving));
        bhput(map, next, 50);
        assert(mlen(map) == 2);
        assert(bhgeti(map, fixed) >= 0 && bhget(map, fixed) == 40);
        assert(bhgeti(map, next) >= 0 && bhget(map, next) == 50);
        moving = next;
    }
    mfree(map);
    puts("pch_ds hash-map regression checks passed.");
    return 0;
}
