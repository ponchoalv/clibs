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
bhinit, shinit or pqinit. Passing NULL uses the heap, so arena-backed and
heap-backed collections can coexist. Keep the arena at the same
address while pch_ds collections reference it. Growth preserves the
collection's contents and can change the collection's address.

arrfree, rfree and mfree clear the handle. Reset the arena to reuse
storage, or destroy it to free storage. Reinitialize collection handles
before using them again after a reset.

Build the integration test with:
  cc -std=gnu89 -Wall -Wextra -Werror test_pch_ds_arena.c -o test_pch_ds_arena

Priority queues (pch_ds.h)
--------------------------

Start with a typed pointer initialized to NULL. Initialize it with
pqinit(queue, arena, cmp). The allocator follows the same rules as
arrinit. The comparator takes two const void pointers to elements and
follows qsort ordering. A negative result puts the first element first.
Use a tie-breaker in cmp if equal priorities need a specific order.

pqpush(queue, value) takes an element by value and returns 1 on success.
pqpeek(queue) gives a pointer to the first element, or NULL when empty.
pqpop(queue, &value) removes the first element. pqremove(queue, i, &value)
removes an element at a heap index. Both return 0 for an invalid index
or empty queue. Pass NULL to discard the removed element, or an output
pointer outside the queue's storage. After changing a queued element's
priority, call pqfix(queue, i) to restore heap order.

pqreserve(queue, capacity) updates the handle when the queue grows.
Check pqcap(queue) to confirm the requested capacity. A failed init
returns NULL. A failed push or reserve leaves an existing queue intact.
Pointers returned by pqpeek can be invalidated by queue mutations.
Use pqlen, pqempty, pqclear and pqfree as with the other collections.

Build the priority queue test with:
  cc -std=gnu89 -Wall -Wextra -Werror test_pch_priority_queue.c -o test_pch_priority_queue
