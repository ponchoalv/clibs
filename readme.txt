Single Header Libs
=================

This are some helper libraries I used, I basically copied Sean Barret
idea and library organisation to write my own. This are not safe or
complete or nothing, I usually use them for solving advent of code
challenges or simple games.

Here is Sean Barret repo link: https://github.com/nothings/stb

Growable arena (pch_arena.h)
----------------------------

Define PCH_ARENA_IMPLEMENTATION in one translation unit. Start with a
zero-initialized PchArena and call pch_arena_alloc for storage. When a
block fills, the arena adds another block without moving live allocations.
pch_arena_resize can grow the last allocation in place, or return a new
allocation with the specified prefix preserved. Allocation failure returns
NULL. pch_arena_reset keeps blocks for reuse, and pch_arena_destroy frees
all blocks.

Build the standalone test with:
  cc -std=gnu89 -Wall -Wextra -Werror test_pch_arena.c -o test_pch_arena

Using a growable arena with pch_ds.h
-----------------------------------

Define PCH_DS_USE_ARENA before including pch_ds.h in each translation
unit that uses pch_ds. Define PCH_ARENA_IMPLEMENTATION and
PCH_DS_IMPLEMENTATION in one translation unit each, as usual.

Pass a pointer to a zero-initialized PchArena to arrinit, rinit,
bhinit or shinit. Passing NULL uses the heap, so arena-backed and
heap-backed collections can coexist. Keep the arena at the same
address while pch_ds collections reference it. Growth preserves the
collection's contents and can change the collection's address.

arrfree, rfree and mfree clear the handle. Reset the arena to reuse
storage, or destroy it to free storage. Reinitialize collection handles
before using them again after a reset.

Build the integration test with:
  cc -std=gnu89 -Wall -Wextra -Werror test_pch_ds_arena.c -o test_pch_ds_arena
