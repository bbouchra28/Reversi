#ifndef MOVES_H
#define MOVES_H

#define MAX_SIZE (size_t) ((size_t) 0 - 1)

#include <stdio.h>

typedef struct
{
  size_t row;
  size_t column;
} move_t;

/* List of moves (forward declaration to hide the implementation) */
typedef struct moves_t moves_t;

/* Functions signatures */
moves_t *moves_alloc(size_t size);
void moves_free(moves_t *moves);
size_t moves_count(moves_t *moves);
move_t moves_get(moves_t *moves, size_t n);
int moves_set(moves_t *moves, size_t n, move_t move);
moves_t *moves_copy(moves_t *moves);
char *move2str(move_t move);
char *moves2str(moves_t *moves);

#endif /* MOVES_H */
