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
