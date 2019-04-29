#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef uint64_t bitmask_t;

struct board_t
{
  size_t  size;
  stone_t player;
  bitmask_t black;
  bitmask_t white;
  bitmask_t hints;
};

/* Functions declarations here! */
board_t *
board_alloc (size_t size, stone_t player)   // allocate memory for the board
{
  if (size < MIN_BOARD_SIZE || size > MAX_BOARD_SIZE || size % 2 != 0)
    return NULL;

  board_t * board = malloc (sizeof(board_t));
  if (!board)
    return NULL;

  board->size = size;
  board->player = player;
  board->white = 0ULL;
  board->black = 0ULL;
  board->hints = 0ULL;
  return board;
}

board_t *
board_init (size_t size)     // initial the board
{
  board_t * board = board_alloc(size, BLACK_STONE);
  if (!board)
    return;
  size_t middle = board->size / 2;
  board_set (board, WHITE_STONE, middle, middle);
  board_set (board, WHITE_STONE, middle - 1, middle - 1);
  board_set (board, BLACK_STONE, middle, middle - 1);
  board_set (board, BLACK_STONE, middle - 1, middle);
  return board;
}

void
board_free (board_t *board)  //free memory
 {
   if (!board)
     return;
   free(board);
 }

 board_t *
 board_copy (board_t *board)  // copy the board
 {
   if (!board)
     return NULL;
   board_t *boardcp = board_alloc (board->size, board->player);
   if (!boardcp)
     return NULL;
   boardcp->black = board->black;
   boardcp->white = board->white;
   //boardcp->hints = board->hints;
   return boardcp;
 }

int
board_set (board_t *board, stone_t stone, size_t row, size_t column) // set the square to stone
{
   if (!board || MIN_BOARD_SIZE > board->size || board->size > MAX_BOARD_SIZE || board->size % 2 != 0)
     return -1;

   if (row > board->size || column > board->size )
     return -1;

   bitmask_t bit_mask = 1ULL << (unsigned int)(column + board->size * row);
   if (stone == BLACK_STONE)
   {
     board->black |= bm;
     board->white &= ~bm;
   }
   else if (stone == WHITE_STONE)
   {
     board->white |= bm;
  	 board->black &= ~bm;
   }
   else if (stone == HINT_STONE)
   {
     board->black &= ~bm;
     board->white &= ~bm;
  	 board->hints |= bm;
   }
   else
   { /* Wrong input */
     fprintf(stderr, "board_set warning: wrong input stone: %c\n", stone);
     return -1;
   }
   return 0;
}

stone_t
board_get (board_t *board, size_t row, size_t column) // return square
{
  if (!board || row >= board->size || column >= board->size)
    return EMPTY_STONE;
  bitmask_t bm = 1ULL << (unsigned int)(column + board->size * row);
  if (board->white & board->black)
  {
    fprintf(stderr, "Invalid argument: board\n");
    exit(EXIT_FAILURE);
  }
  if (board->black & bm)
  {
    return BLACK_STONE;
  }
  if (board->white & bm)
  {
    return WHITE_STONE;
  }
}

score_t
board_score (board_t *board) // return score
{
   score_t score = {0, 0};
   for (size_t i = 0; i < board->size; i++)
     for (size_t j = 0; j < board->size; j++)
     {
       if (board->gri == WHITE_STONE)
         (score.white)++;
       if (board->grid[i][j] == BLACK_STONE)
         (score.black)++;
     }
   return score;
}

size_t
board_get_size (board_t *board)    // return board_size
{
  return board->size;
}

stone_t
board_get_player (board_t *board)   // return player
{
  return board->player;
}

char *
board_str (board_t *board)  // return the board in a string form
{
  if (!board)
    return NULL;
  size_t sizestr = (board->size + 1) * (2 * (board->size + 1));
  char * boardstr = malloc (sizestr * sizeof(char));
  if (!boardstr)
  {
    free(boardstr);
    return NULL;
  }

  for (size_t i = 0; i < board->size; i++)
  {
      for (size_t j = 0; j < board->size; j++)
  	  {
  	    move_t move = {i, j};
  	    if ((board->grid[i][j] == EMPTY_STONE) && (board_is_move_valid (board, move)))
  	      board->grid[i][j] = HINT_STONE;
  	}
  }

  char c = 'A';
  boardstr[0] = ' ';
  int x = 0;
  for (size_t i = 2; i <= 2 * board->size; i = i + 2)
  {
    boardstr[i - 1] = ' ';
    boardstr[i] = x + '0';
    x++;
  }
  boardstr[2 * board->size + 1] = '\n';
  size_t y = 2 * (board->size + 1);

  for (size_t j = 0; j < board->size; j++)
  {
    boardstr[y] = c;
    c++;
    y++;
    for (size_t k = 0; k < board->size; k++)
    {
      boardstr[y] = ' ';
      boardstr[y + 1] = board->grid[j][k];
      y = y + 2;
    }
    boardstr[y] = '\n';
    y++;
  }
  return boardstr;
}

void
board_print (board_t *board, FILE *fd)   // print board on file
{
  char * boardstr = board_str (board);
  for (size_t i = 0; i < strlen (boardstr); i++)
    fputc(boardstr[i], fd);

  score_t score = board_score(board);
  fprintf(fd, "\nScore: 'X' = %lu, 'O' = %lu", score.black, score.white);
}

static bool
is_top_left_diag_valid (board_t *board, move_t move)
{
  int row = move.row;
  int column = move.column;

  if ((row - 1 < 0) || (column - 1 < 0))
    return false;

  row--;
  column--;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      row--;
      column--;
      if ((row < 0) || (column < 0))
	      return false;

      sp = board->grid[row][column];
      if ((sp == EMPTY_STONE) || (sp == HINT_STONE))
	      return false;
    }

  return true;
}

static bool
is_down_left_diag_valid (board_t *board, move_t move)
{
  int row = move.row;
  int column = move.column;

  if ((row + 1 == board->size) || (column - 1 < 0))
    return false;

  row++;
  column--;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      row++;
      column--;
      if ((row == board->size) || (column < 0))
	return false;

      sp = board->grid[row][column];
      if ((sp == EMPTY_STONE) || (sp == HINT_STONE))
	return false;
    }

  return true;
}

static bool
is_top_right_diag_valid (board_t *board, move_t move)
{
  int row = move.row;
  int column = move.column;

  if ((row - 1 < 0) || (column + 1 == board->size))
    return false;

  row--;
  column++;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      row--;
      column++;
      if ((row < 0) || (column == board->size))
	      return false;

      sp = board->grid[row][column];
      if ((sp == EMPTY_STONE) || (sp == HINT_STONE))
	      return false;
    }

  return true;
}

static bool
is_down_right_diag_valid (board_t *board, move_t move)
{
  int row = move.row;
  int column = move.column;

  if ((row + 1 == board->size) || (column + 1 == board->size))
    return false;

  row++;
  column++;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      row++;
      column++;
      if ((row == board->size) || (column == board->size))
	      return false;

      sp = board->grid[row][column];
      if ((sp == HINT_STONE) || (sp == EMPTY_STONE))
	      return false;
    }

  return true;
}

static bool
is_top_valid (board_t *board, move_t move)
{
  int row = move.row;
  int column = move.column;

  if (row - 1 <  0)
    return false;

  row--;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      row--;
      if (row < 0)
	      return false;

      sp = board->grid[row][column];
      if ((sp == HINT_STONE) || (sp == EMPTY_STONE))
	      return false;
    }

  return true;
}

static bool
is_down_valid (board_t *board, move_t move)
{
  int row = move.row;
  int column = move.column;

  if (row + 1 == board->size)
    return false;

  row++;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      row++;
      if (row == board->size)
	      return false;

      sp = board->grid[row][column];
      if ((sp == HINT_STONE) || (sp == EMPTY_STONE))
	      return false;
    }

  return true;
}

static bool
is_right_valid (board_t *board, move_t move)
{
  int row = move.row;
  int column = move.column;

  if (column + 1 == board->size)
    return false;

  column++;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      column++;
      if (column == board->size)
	      return false;

      sp = board->grid[row][column];
      if ((sp == HINT_STONE) || (sp == EMPTY_STONE))
	      return false;
    }

  return true;
}

static bool
is_left_valid (board_t *board, move_t move)
{
  int row = move.row;
  int column = move.column;

  if (column - 1 < 0)
    return false;

  column--;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      column--;
      if (column < 0)
	      return false;

      sp = board->grid[row][column];
      if ((sp == HINT_STONE) || (sp == EMPTY_STONE))
	      return false;
    }

  return true;
}

bool board_is_move_valid (board_t *board, move_t move)
{
  size_t row = move.row;
  size_t column = move.column;

  if (!board)
    return false;

  if ((row >= board->size) || (column >= board->size))
    return false;

  return  is_left_valid (board, move) || is_right_valid (board, move) ||
          is_down_valid (board, move) || is_top_valid (board, move) ||
          is_top_right_diag_valid (board, move) || is_down_right_diag_valid (board, move) ||
          is_top_left_diag_valid (board, move) || is_down_left_diag_valid(board, move);
}

bool
board_play (board_t *board, move_t move)
{
  if ((!board) || (!board_is_move_valid(board, move)))
    return false;

  size_t size = board_get_size(board);
  size_t row = move.row;
  size_t column = move.column;
  size_t row1;
  size_t column1;
  stone_t current_player = board_get_player(board);
  stone_t next_player;

  if (current_player == BLACK_STONE)
    {
      next_player = WHITE_STONE;
    }
  if (current_player == WHITE_STONE)
    {
      next_player = BLACK_STONE;
    }

  if (is_top_valid (board, move))
    {
      column1 = column;
      row1 = row;
      board_set (board, current_player, row1, column1);
      stone_t sp = board->grid[row1 - 1][column1];

      while (sp != current_player)
	{
	  board_set (board, current_player, row1, column1);
	  row1--;
	  sp = board->grid[row1][column1];
	}
    }

  if (is_down_valid (board, move))
    {
      column1 = column;
      row1 = row;
      board_set (board, current_player, row1, column1);
      stone_t sp = board->grid[row1 + 1][column1];

      while (sp != current_player)
	{
	  board_set (board, current_player, row1, column1);
	  row1++;
	  sp = board->grid[row1][column1];
	}
    }

  if (is_left_valid (board, move))
    {
      column1 = column;
      row1 = row;
      board_set (board, current_player, row1, column1);
      stone_t sp = board->grid[row1][column1 - 1];

      while (sp != current_player)
	{
	  board_set (board, current_player, row1, column1);
	  column1--;
	  sp = board->grid[row1][column1];
	}
    }

  if (is_right_valid (board, move))
    {
      column1 = column;
      row1 = row;
      board_set (board, current_player, row1, column1);
      stone_t sp = board->grid[row1][column1 + 1];

      while (sp != current_player)
	{
	  board_set (board, current_player, row1, column1);
	  column1++;
	  sp = board->grid[row1][column1];
	}
    }

  if (is_top_left_diag_valid (board, move))
    {
      column1 = column;
      row1 = row;
      board_set (board, current_player, row1, column1);
      stone_t sp = board->grid[row1 - 1][column1 - 1];

      while (sp != current_player)
	{
	  board_set (board, current_player, row1, column1);
	  row1--;
	  column1--;
	  sp = board->grid[row1][column1];
	}
    }

  if (is_down_left_diag_valid (board, move))
    {
      column1 = column;
      row1 = row;
      board_set (board, current_player, row1, column1);
      stone_t sp = board->grid[row1 + 1][column1 - 1];

      while (sp != current_player)
	{
	  board_set (board, current_player, row1, column1);
	  row1++;
	  column1--;
	  sp = board->grid[row1][column1];
	}
    }

  if (is_top_right_diag_valid (board, move))
    {
      column1 = column;
      row1 = row;
      board_set (board, current_player, row1, column1);
      stone_t sp = board->grid[row1 - 1][column1 + 1];

      while (sp != current_player)
	{
	  board_set (board, current_player, row, column);
	  row1--;
	  column1++;
	  sp = board->grid[row1][column1];
	}
    }

  if (is_down_right_diag_valid (board, move))
    {
      column1 = column;
      row1 = row;
      board_set (board, current_player, row1, column1);
      stone_t sp = board->grid[row1 + 1][column1 + 1];

      while (sp != current_player)
	{
	  board_set (board, current_player, row1, column1);
	  row1++;
	  column1++;
	  sp = board->grid[row1][column1];
	}
    }

  for (size_t i = 0; i < size; i++)
    {
      for (size_t j = 0; j < size; j++)
	{
	  if (board->grid[i][j] == HINT_STONE)
	    {
	      board->grid[i][j] = EMPTY_STONE;
	    }
	}
    }

  board->player = next_player;
  int cpt = 0;

  move_t move_c;
  for (size_t a = 0; a < size; a++)
    {
      for (size_t b = 0; b < size; b++)
  	{
  	  move_c.row = a;
  	  move_c.column = b;
  	  if ((board->grid[a][b] == EMPTY_STONE) &&
	      (board_is_move_valid (board, move_c)))
	    {
	      board->grid[a][b] = HINT_STONE;
	      cpt++;
	    }
  	}
    }

  if (cpt == 0)
    board->player = EMPTY_STONE;

  return true;
}


void
board_save (board_t *board, FILE *fd)
{
  fputc (board_get_player (board), fd);
  fputc ('\n', fd);
  fputs (board_str (board), fd);
}
/*
board_t *
board_load (char *filename)
{
  board_t *board;
  char *line;
  FILE *fd = fopen(filename, "r");
  if(!fd)
  {
    printf("error in opening file\n");
  }
  while(fgets(line, strlen(line), fd) != NULL)
  {
    if (line[0] == '#' || line[0] == SPACES)
        continue;
    if (line[0] == 'X' || line[0] == 'O')
     //{
    //   printf("%c", line[0]);
     //}
    //printf("%s",line);
  }
}
*/
moves_t *
board_get_moves (board_t *board)
{
  return board->moves;
}

int
board_count_moves (board_t *board)
{
  moves_t *moves = board_get_moves (board);
  return moves_count (moves);
}
