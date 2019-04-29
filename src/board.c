#include "board.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct board_t
{
  size_t  size;
  stone_t player;
  stone_t **grid;
  moves_t *moves;
};

/* Functions declarations here! */
board_t *
board_alloc (size_t size, stone_t player)   // allocate memory for the board
{
  if((size > MAX_BOARD_SIZE) || (size < MIN_BOARD_SIZE))
    return NULL;
  board_t *board = malloc (sizeof(board_t));
  if (!board)
    return NULL;
  board->size = size;
  board->player = player;
  board->grid = malloc (sizeof(stone_t *) * size);
  if (!board->grid)
  {
    free(board->moves);
    free(board->grid);
    free (board);
    return NULL;
  }
  for (size_t i = 0; i < size; i++)
  {
      board->grid[i] = malloc (sizeof(stone_t) * size);
      if (!board->grid[i])
      {
        for (size_t j = 0; i < j; j++)
        {
          free (board->grid[j]);
        }
      }
  }
  return board;
}

board_t *
board_init (size_t size)     // initial the board
{
  if (!size || (size % 2 != 0) || (size > MAX_BOARD_SIZE))
    return NULL;
  board_t *board;
  if (size == 2)
    board = board_alloc (size, EMPTY_STONE);
  else
    board = board_alloc (size, BLACK_STONE);
  if (!board)
    return NULL;

  for (size_t i = 0; i < board->size; i++)
      for (size_t j = 0; j < board->size; j++)
	      board->grid[i][j] = EMPTY_STONE;

  size_t middle = board->size / 2;
  board_set (board, WHITE_STONE, middle, middle);
  board_set (board, WHITE_STONE, middle - 1, middle - 1);
  board_set (board, BLACK_STONE, middle, middle - 1);
  board_set (board, BLACK_STONE, middle - 1, middle);

  board->moves = moves_alloc (board_count_moves(board));
  int pos = 0;
  for (size_t i = 0; i < board->size; i++)
  {
      for (size_t j = 0; j < board->size; j++)
	    {
	       move_t move = {i, j};
	       if (board_is_move_valid (board, move))
	       {
	          moves_set (board->moves, pos, move);
	          pos++;
	       }
	     }
  }
  return board;
}

void
board_free (board_t *board)  //free memory
 {
   if (!board)
     return;
   if (board)
   {
     if (board->grid)
       for (size_t i = 0; i < board->size ; i++)
         free(board->grid[i]);
     free(board->grid);
     moves_free (board->moves);
     free(board);
   }
 }

 board_t *
 board_copy (board_t *board)  // copy the board
 {
   if (!board)
     return NULL;

   stone_t player = board_get_player (board);
   board_t *boardcp = board_alloc (board->size, player);
   if (!boardcp)
      return NULL;

   for (size_t i = 0; i < boardcp->size; i++)
     for (size_t j = 0; j < boardcp->size; j++)
       boardcp->grid[i][j] = board->grid[i][j];
   boardcp->moves = moves_copy (board->moves);
   return boardcp;
 }

// set the square to stone
int
board_set (board_t *board, stone_t stone, size_t row, size_t column)
{
   if (!board || MIN_BOARD_SIZE > board->size || board->size > MAX_BOARD_SIZE
       || board->size % 2 != 0)
     return -1;

   if (row > board->size || column > board->size )
     return -1;

   board->grid[row][column] = stone;

   return 0;
}

 stone_t
 board_get (board_t *board, size_t row, size_t column) // return square
 {
   if (!board || row >= board->size || column >= board->size)
     return EMPTY_STONE;
   return board->grid[row][column];
 }

score_t
board_score (board_t *board) // return score
{
   score_t score = {0, 0};
   for (size_t i = 0; i < board->size; i++)
     for (size_t j = 0; j < board->size; j++)
     {
       if (board->grid[i][j] == WHITE_STONE)
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

void
board_set_player (board_t *board, stone_t player)   // set player
{
  board->player = player;
}

char *
board_str (board_t *board)  // return the board in a string form
{
  if (!board || !board->grid)
    return NULL;
  size_t size = board_get_size(board);
  size_t sizestr = (2 + size * 2) * (size + 1) ;
  char *boardstr = '\0';
  boardstr = malloc (sizestr * sizeof(char));
  if (!boardstr)
    return NULL;

  for (size_t i = 0; i < board->size; i++)
  {
      for (size_t j = 0; j < board->size; j++)
  	  {
  	    move_t move = {i, j};
  	    if ((board->grid[i][j] == EMPTY_STONE)
        && (board_is_move_valid (board, move)))
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
  size_t y = 2 * (board->size + 1) ;

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
  if (!fd)
    exit(EXIT_FAILURE);
  char *boardstr = board_str(board);
  size_t sizestr = (2 + board->size * 2) * (board->size + 1) ;
  for( size_t i = 0; i < sizestr; i++)
    fputc(boardstr[i], fd);
  free(boardstr);
  score_t score = board_score(board);
  fprintf(fd, "\nScore: 'X' = %lu, 'O' = %lu", score.black, score.white);
}

static bool
is_top_left_diag_valid (board_t *board, move_t move)
{
  size_t row = move.row;
  size_t column = move.column;

  if (((int)row - 1 < 0) || ((int)column - 1 < 0))
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
      if (((int)row < 0) || ((int)column < 0))
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
  size_t row = move.row;
  size_t column = move.column;

  if ((row + 1 == board->size) || ((int)column - 1 < 0))
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
      if ((row == board->size) || ((int)column < 0))
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
  size_t row = move.row;
  size_t column = move.column;

  if (((int)row - 1 < 0) || (column + 1 == board->size))
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
      if (((int)row < 0) || (column == board->size))
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
  size_t row = move.row;
  size_t column = move.column;

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
  size_t row = move.row;
  size_t column = move.column;

  if ((int)row - 1 <  0)
    return false;

  row--;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      row--;
      if ((int)row < 0)
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
  size_t row = move.row;
  size_t column = move.column;

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
  size_t row = move.row;
  size_t column = move.column;

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
  size_t row = move.row;
  size_t column = move.column;

  if ((int)column - 1 < 0)
    return false;

  column--;
  stone_t sp = board->grid[row][column];
  if ((sp == board->player) || (sp == HINT_STONE) || (sp == EMPTY_STONE))
    return false;

  while (sp != board->player)
    {
      column--;
      if ((int)column < 0)
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
          is_top_right_diag_valid (board, move) ||
          is_down_right_diag_valid (board, move) ||
          is_top_left_diag_valid (board, move) ||
          is_down_left_diag_valid(board, move);
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
  for (size_t i = 0; i < size; i++)
    {
      for (size_t j = 0; j < size; j++)
      	{
      	  move_c.row = i;
      	  move_c.column = j;
      	  if ((board->grid[i][j] == EMPTY_STONE) &&
    	      (board_is_move_valid (board, move_c)))
      	    {
      	      board->grid[i][j] = HINT_STONE;
      	      cpt++;
      	    }
      	}
    }
    if (board->moves)
      moves_free(board->moves);
    board->moves = moves_alloc (cpt);
    size_t pos = 0;
    for (size_t i = 0; i < size; i++)
      {
        for (size_t j = 0; j < size; j++)
        	{
        	  move_c.row = i;
        	  move_c.column = j;
        	  if (board->grid[i][j] == HINT_STONE)
            {
                moves_set(board->moves, pos, move_c);
        	      pos++;
            }
        	}
      }
  if (cpt == 0)
    board->player = EMPTY_STONE;

  return true;
}

moves_t *
board_get_moves (board_t *board)
{
  return board->moves;
}

int
board_count_moves (board_t *board)
{
  if (!board || !board->grid)
    return 0;
  move_t move;
  int cpt = 0;
  for (size_t i = 0; i < board->size; i++)
  {
      for (size_t j = 0; j < board->size; j++)
	      {
        	  move.row = i;
            move.column = j;
        	  if (board->grid[i][j] == EMPTY_STONE &&
        	       board_is_move_valid (board, move))
        	    {
        	      cpt++;
        	    }
	     }
  }

  return cpt;
}
