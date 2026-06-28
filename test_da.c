#define AOC_IMPLEMENTATION
#include "aoc.h"

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} IntArray;

int main() {
    Arena arena = arena_create(1000 * mega_byte);
    IntArray arr = {0};

    size_t i;
    int it;
    for (i = 0; i < 10; i++) {
        aoc_da_append(&arr, i*i, &arena);
    }
    // aoc_da_remove_unordered(&arr, 0);
    // int items[10];

    aoc_da_foreach(i, it, &arr) {
        printf("i: %zu, value: %d\n", i, it);
    }
    
    int item;
    aoc_da_shift(&arr, item);
    // aoc_da_shift(&arr, item);
    
    printf("item shifted %d\n", item);

    aoc_da_delat(&arr, 2, item);
    
    printf("item removed at 2 %d\n", item);

      aoc_da_foreach(i, it, &arr) {
        printf("i: %zu, value: %d\n", i, it);
    }

    // aoc_da_insertn(&arr, 2, 5, &arena);
    aoc_da_insat(&arr, 2, 99, &arena);
    aoc_da_foreach(i, it, &arr) {
        printf("i: %zu, value: %d\n", i, it);
    }

    arena_destroy(&arena);
    return 0;
}