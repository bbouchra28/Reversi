#include "moves.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct moves_t
{
  size_t size;
  move_t *list;
};

/* Functions Declaration */
moves_t *
moves_alloc (size_t size)
{
  if (size > MAX_SIZE || size < 1)
    return NULL;

  moves_t *moves = malloc(sizeof(moves_t));
  if (!moves)
    return NULL;

  moves->size = size;
  moves->list = malloc(sizeof(move_t) * size);
  if (!moves->list)
  {
    free(moves);
    return NULL;
  }
  return moves;
}

void
moves_free (moves_t *moves)
{
  if (!moves)
    return;
  if (moves->list)
      free(moves->list);
  free(moves);
}

size_t
moves_count (moves_t *moves)
{
  if (!moves)
    return 0;
  return(moves->size);
}

move_t
moves_get (moves_t *moves, size_t n)
{
  move_t move;
  if (!moves || n >= moves->size )
  {
    move.row = MAX_SIZE;
    move.column = MAX_SIZE;
    return move;
  }
  return moves->list[n];
}

int
moves_set (moves_t *moves, size_t n, move_t move)
{
  if (!moves|| n >= moves->size)
    return -1;
  moves->list[n].row = move.row;
  moves->list[n].column = move.column;
  return 0;
}

moves_t *
moves_copy (moves_t *moves)
{
  if (!moves)
    return NULL;
  moves_t *movescp = moves_alloc(moves->size);
  if (!movescp)
    return NULL;
  for (size_t i = 0; i < moves->size; i++)
    moves_set(movescp, i, moves_get(moves, i));
  return movescp;
}

char *
move2str (move_t move)
{
  char *movestr = malloc(sizeof(char) * 7);
  if (!movestr)
    return NULL;
  movestr[0] = '{';
  movestr[1] = move.row + 'A';
  movestr[2] =',';
  movestr[3] = ' ';
  movestr[4] = move.column + '0';
  movestr[5] = '}';
  movestr[6] = '\0';
  return movestr;
}

char *
moves2str (moves_t *moves)
{
  if (!moves)
    return NULL;
  char *movesstr = malloc((7 * moves->size + 15) * sizeof(char));
  movesstr[0] = '\0';
  if (!movesstr)
    return NULL;
  strncat(movesstr, "{", 1);
  for (size_t i = 0; i < moves->size; i++)
  {
    char *movestr = move2str(moves_get(moves, i));
    strncat(movesstr, movestr, 6);
    strncat(movesstr, " ", 1);
    free (movestr);
  }
  strncat(movesstr, "}\0", 2);
  return movesstr;
}
