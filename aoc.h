#ifndef AOC_H_
#define AOC_H_
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#define ARENA_IMPLEMENTATION
#include "arena.h"
#define MAX_INPUT_FAILED 50

#ifndef BOOL_H_
#define BOOL_H_
#define aoc_bool int
#define aoc_false 0
#define aoc_true 1
#endif

typedef struct {
  int c;
  int r;
  size_t count;
  size_t capacity;
  char *items;
} AOC_Grid;

typedef struct {
  int r;
  int c;
} AOC_Vec2;

typedef struct {
  int dr;
  int dc;
} AOC_Direction;

typedef struct {
  size_t count;
  size_t capacity;
  size_t *items;
} AOC_Path_leghts;

typedef struct {
  AOC_Vec2 pos;
  size_t *neighbors;
  size_t *distances;
  size_t neighbor_count;
  size_t neighbor_capacity;
} AOC_Graph_Node;

typedef struct {
  AOC_Graph_Node *nodes;
  size_t count;
  size_t capacity;
} AOC_Contracted_Graph;

const AOC_Vec2 all_directions[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

typedef size_t (*aoc_get_neighbours_fn)(const AOC_Grid *grid, AOC_Vec2 pos,
                                        const char *wall,
                                        const AOC_Vec2 *directions,
                                        const size_t dir_count,
                                        AOC_Vec2 *neighbours, void *user_data);

typedef struct {
  aoc_get_neighbours_fn get_neighbours; /* Custom neighbor function */
  const char *wall_chars;               /* Characters considered as walls */
  const AOC_Vec2 *directions;           /* Available movement directions */
  size_t direction_count;               /* Number of directions */
  void *user_data; /* User-defined data for neighbor function */
} AOC_PathfindConfig;

typedef struct AOC_QueueNode AOC_QueueNode;
typedef struct AOC_Queue AOC_Queue;

struct AOC_QueueNode {
  char tile;
  AOC_Vec2 pos;
  AOC_QueueNode *next;
};

struct AOC_Queue {
  AOC_QueueNode *head;
  AOC_QueueNode *tail;
  size_t size;
};

/* AOC GRID FUNCTIONS */
/* TODO: use an arean allocator for the grid */
AOC_Grid aoc_grid_create(int r, int c, char elem);
int aoc_parse_grid_from_input(AOC_Grid *grid, const char *input, Arena *arena);
aoc_bool aoc_grid_append(AOC_Grid *grid, const char elem, Arena *arena);
char aoc_grid_set(AOC_Grid *grid, int r, int c, char chr);
void aoc_grid_destroy(AOC_Grid *grid);
aoc_bool aoc_grid_get_at(const AOC_Grid *grid, const int r, const int c,
                         char *out);
void aoc_grid_print(const AOC_Grid *grid);
aoc_bool aoc_grid_find(const AOC_Grid *grid, char chr, AOC_Vec2 *out);
aoc_bool aoc_grid_find_at_row(const AOC_Grid *grid, int r, const char chr,
                              AOC_Vec2 *out);

/* String utility functions */
char **aoc_split_values(const char *values_str, long *count, Arena *arena);

/* AOC QUEUE FUNCTIONS */
void aoc_queue_init(AOC_Queue *queue);
AOC_QueueNode *aoc_queue_pop(AOC_Queue *queue);
AOC_QueueNode *aoc_queue_peek(const AOC_Queue *queue);
size_t aoc_queue_size(const AOC_Queue *queue);
aoc_bool aoc_queue_is_empty(const AOC_Queue *queue);
void aoc_queue_push(AOC_Queue *queue, AOC_QueueNode *node);

/* memory management */

/* C89 compatible macros - use default alignment for portability */
#define malloc(A, P, N) arena_malloc(A, N, sizeof(void *))
#define calloc_a(A, P, N) arena_calloc(A, N, sizeof(*(P)), sizeof(void *))
#define realloc(A, P, N) arena_realloc(A, P, N, sizeof(void *))
#define memdup(A, P, N) arena_memdup(A, P, N, sizeof(void *))
#define strdup(A, S) memdup(A, S, strlen(S) + 1)
#define strapp(A, S1, S2)                                                      \
  strcat(realloc(A, S1, strlen(S1) + strlen(S2) + 1), S2)

/* Dynamic Array Implementation took it from https://github.com/tsoding/nob.h */
#ifndef AOC_DA_INIT_CAP
#define AOC_DA_INIT_CAP 256
#endif

#define aoc_da_reserve(da, expected_capacity, arena)                           \
  do {                                                                         \
    if ((expected_capacity) > (da)->capacity) {                                \
      if ((da)->capacity == 0) {                                               \
        (da)->capacity = AOC_DA_INIT_CAP;                                      \
      }                                                                        \
      while ((expected_capacity) > (da)->capacity) {                           \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items = realloc((arena), (da)->items,                              \
                            (da)->capacity * sizeof(*(da)->items));            \
      assert((da)->items != NULL && "No more ram for you");                    \
    }                                                                          \
  } while (0)

/* Append an item to a dynamic array */
#define aoc_da_append(da, item, arena)                                         \
  do {                                                                         \
    aoc_da_reserve((da), (da)->count + 1, (arena));                            \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#define aoc_da_free(da) free((da).items)

/* Append several items to a dynamic array */
#define aoc_da_append_many(da, new_items, new_items_count, arena)              \
  do {                                                                         \
    aoc_da_reserve((da), (da)->count + (new_items_count), (arena));            \
    memcpy((da)->items + (da)->count, (new_items),                             \
           (new_items_count) * sizeof(*(da)->items));                          \
    (da)->count += (new_items_count);                                          \
  } while (0)

#define aoc_da_resize(da, new_size, arena)                                     \
  do {                                                                         \
    aoc_da_reserve((da), (new_size), (arena));                                 \
    (da)->count = (new_size);                                                  \
  } while (0)

#define aoc_da_last(da) (da)->items[(assert((da)->count > 0), (da)->count - 1)]

#define aoc_da_remove_unordered(da, i)                                         \
  do {                                                                         \
    size_t j = (i);                                                            \
    assert(j < (da)->count);                                                   \
    (da)->items[j] = (da)->items[--(da)->count];                               \
  } while (0)

#define aoc_da_insertn(da, idx, n, arena)                                      \
  do {                                                                         \
    assert((idx) < (da)->count);                                               \
    aoc_da_reserve((da), (da)->count + (n), (arena));                          \
    memmove(&(da)->items[(idx) + (n)], &(da)->items[(idx)],                    \
            sizeof(*(da)->items) * ((da)->count - (idx)));                     \
    (da)->count += (n);                                                        \
  } while (0)

#define aoc_da_insat(da, idx, item, arena)                                     \
  do {                                                                         \
    aoc_da_insertn((da), (idx), 1, (arena));                                   \
    (da)->items[(idx)] = (item);                                               \
  } while (0)

#define aoc_da_delat(da, idx, item)                                            \
  do {                                                                         \
    assert((idx) < (da)->count);                                               \
    item = (da)->items[(idx)];                                                 \
    memmove(&(da)->items[(idx)], &((da)->items[(idx) + 1]),                    \
            sizeof((da)->items[(idx)]) * (--(da)->count - (idx)));             \
  } while (0)

#define aoc_da_shift(da, item) aoc_da_delat((da), 0, (item))

/* Foreach over Dynamic Arrays. Example: */
/* ```c */
/* typedef struct { */
/*     int *items; */
/*     size_t count; */
/*     size_t capacity; */
/* } Numbers; */

/* Numbers xs = {0}; */

/* aoc_da_append(&xs, 69); */
/* aoc_da_append(&xs, 420); */
/* aoc_da_append(&xs, 1337); */
#define aoc_da_foreach(i, it, da)                                              \
  for ((i) = 0, (it) = (da)->items[(i)]; (i) < (da)->count;                    \
       (i)++, it = (da)->items[(i)])

/* Some simple math stuff for aoc */
/**
 * Fast GCD using Euclidean algorithm
 */

long gcd(long a, long b);
/**
 * LCM using GCD
 */
long lcm(long a, long b);

#endif /* AOC_H_ */

#ifdef AOC_IMPLEMENTATION
AOC_Grid aoc_grid_create(int r, int c, char elem) {
  AOC_Grid grid;
  grid.r = r;
  grid.c = c;
  grid.count = 0;
  grid.items = calloc(r * c, sizeof(elem));
  memset(grid.items, elem, r * c);
  return grid;
}

int aoc_parse_grid_from_input(AOC_Grid *grid, const char *input, Arena *arena) {
  int result = 0;
  char current;
  size_t char_count;

  FILE *fd = fopen(input, "r");
  if (!fd) {
    char msg[MAX_INPUT_FAILED] = "failed to open input file ";
    strncat(msg, input, MAX_INPUT_FAILED - strlen(msg) - 1);
    perror(msg);
    result = errno;
    goto clean;
  }
  char_count = 0;
  while ((current = fgetc(fd)) != EOF) {
    if (current != '\n') {
      aoc_grid_append(grid, current, arena);
      char_count++;
    } else {
      grid->r++;
      grid->c = char_count;
      char_count = 0;
    }
  }
clean:
  if (fd) {
    fclose(fd);
  }
  return result;
}

aoc_bool aoc_grid_append(AOC_Grid *grid, const char elem, Arena *arena) {
  if (grid->count >= grid->capacity) {
    size_t new_capacity = grid->capacity ? grid->capacity * 2 : AOC_DA_INIT_CAP;

    char *tmp = realloc(arena, grid->items, new_capacity);
    if (!tmp) {
      printf("ERROR: Grid allocation failed! arena=%p, items=%p, "
             "new_capacity=%zu\n",
             (void *)arena, (void *)grid->items, new_capacity);
      return aoc_false;
    }
    grid->items = tmp;
    grid->capacity = new_capacity;
  }

  grid->items[grid->count] = elem;
  grid->count++;
  return aoc_true;
}

char aoc_grid_set(AOC_Grid *grid, int r, int c, char chr) {
  char old;
  if (grid->r <= r || grid->c <= c) {
    return 0;
  }
  old = grid->items[(r * grid->c) + c];
  grid->items[(r * grid->c) + c] = chr;
  return old;
}

void aoc_grid_destroy(AOC_Grid *grid) {
  /* Arena memory is freed when arena is destroyed, no individual free needed */
  (void)grid;
}

aoc_bool aoc_grid_get_at(const AOC_Grid *grid, const int r, const int c,
                         char *out) {
  if (grid->r <= r || grid->c <= c) {
#ifndef NDEBUG
    printf("[DEBUG] r:%d c:%d one of both of them is/are invalid.\n", r, c);
#endif
    return aoc_false;
  }

  *out = grid->items[(r * grid->c) + c];
  return aoc_true;
}

void aoc_grid_print(const AOC_Grid *grid) {
  int r = 0;
  int c = 0;
  char found;
  for (r = 0; r < grid->r; r++) {
    for (c = 0; c < grid->c; c++) {
      if (aoc_grid_get_at(grid, r, c, &found)) {
        putchar(found);
      }
    }
    putchar('\n');
  }
}

aoc_bool aoc_grid_find(const AOC_Grid *grid, const char chr, AOC_Vec2 *out) {
  int r = 0;
  for (r = 0; r < grid->r; r++) {
    if (aoc_grid_find_at_row(grid, r, chr, out)) {
      return aoc_true;
    }
  }
  return aoc_false;
}

aoc_bool aoc_grid_find_at_row(const AOC_Grid *grid, int r, const char chr,
                              AOC_Vec2 *out) {
  AOC_Vec2 p = {0};
  int c = 0;
  for (c = 0; c < grid->c; c++) {
    if (grid->items[(r * grid->c) + c] == chr) {
      p.r = r;
      p.c = c;
      *out = p;
      return aoc_true;
    }
  }
  return aoc_false;
}

char **aoc_split_values(const char *values_str, long *count, Arena *arena) {
  long len = strlen(values_str);
  char **values;
  char *value_buffer;
  long max_values = 16;
  long value_idx = 0;
  long i, value_start, value_len;

  values = malloc(arena, values, max_values);

  i = 0;
  while (i < len) {
    /* Skip spaces */
    while (i < len && values_str[i] == ' ') {
      i++;
    }

    if (i >= len)
      break;

    /* Find value boundaries */
    value_start = i;
    while (i < len && values_str[i] != ' ') {
      i++;
    }
    value_len = i - value_start;

    if (value_len > 0) {
      /* Reallocate if needed */
      if (value_idx >= max_values) {
        max_values *= 2;
        values =
            arena_malloc(arena, max_values * sizeof(char *), sizeof(void *));
      }

      /* Copy value to arena-allocated buffer */
      value_buffer =
          arena_malloc(arena, value_len + 1 * sizeof(char), sizeof(void *));
      memcpy(value_buffer, values_str + value_start, value_len);
      value_buffer[value_len] = '\0';

      values[value_idx++] = value_buffer;
    }
  }

  *count = value_idx;
  return values;
}

/* Some simple math stuff for aoc */
/**
 * Fast GCD using Euclidean algorithm
 */
long gcd(long a, long b) {
  long temp;
  a = labs(a);
  b = labs(b);
  while (b != 0) {
    temp = b;
    b = a % b;
    a = temp;
  }
  return a;
}

/**
 * LCM using GCD
 */
long lcm(long a, long b) { return labs(a * b) / gcd(a, b); }

/* // void aoc_queue_init(AOC_Queue *queue)
// {
//   queue->head = NULL;
//   queue->tail = NULL;
//   queue->size = 0;
// }

// AOC_QueueNode *aoc_queue_peek(const AOC_Queue *queue)
// {
//   if (queue->head == NULL)
//   {
//     return NULL;
//   }
//   return queue->head;
// }

// AOC_QueueNode *aoc_queue_pop(AOC_Queue *queue)
// {
//   if (queue->head == NULL)
//   {
//     return NULL;
//   }

//   AOC_QueueNode *node = queue->head;
//   queue->head = queue->head->next;
//   if (queue->head == NULL)
//   {
//     queue->tail = NULL;
//   }
//   queue->size--;
//   return node;
// }

// size_t aoc_queue_size(const AOC_Queue *queue)
// {
//   return queue->size;
// }

// aoc_bool aoc_queue_is_empty(const AOC_Queue *queue)
// {
//   return queue->size == 0;
// }

// void aoc_queue_push(AOC_Queue *queue, AOC_QueueNode *node)
// {
//   if (queue->tail)
//   {
//     queue->tail->next = node;
//   }
//   else
//   {
//     queue->head = node;
//   }
//   queue->tail = node;
//   queue->size++;
// }
 */
#endif /* AOC_IMPLEMENTATION */
