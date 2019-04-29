#include "player.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "board.h"

static int get_min (stone_t, board_t *, size_t);
static int get_min_ab (stone_t player, board_t *board, int alpha, int beta,
                    size_t depth);

void
board_save (board_t *board, FILE *fd)
{
  if(fd)
  {
    fputc (board_get_player (board), fd);
    char *str = board_str (board);
    size_t size = board_get_size(board);
    size_t sizestr = (2 + size * 2) * (size + 1);
    for (size_t i = 0; i < sizestr; i++)
    {
      if (str[i] == 'X' || str[i] == 'O' || str[i] == '_' || str[i] == '\n'
         || str[i] == ' ')
        fputc(str[i], fd);
      else if (str[i] == '*')
        fputc('_', fd);
    }
    free(str);
  }
  else
  {
    exit(EXIT_FAILURE);
  }
}

move_t
random_player(board_t *board, size_t param)
{
    if (!board)
      exit(EXIT_FAILURE);
    moves_t *moves = board_get_moves (board);
    size_t size = moves_count (moves);
    if (size == 0)
      exit(EXIT_FAILURE);
    srand (time(NULL));
    size_t rd = rand() % size ;
    return moves_get (moves, rd);
}

static void
removeSpaces(char *str)
{
    int count = 0;
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ')
            str[count++] = str[i];
    str[count] = '\0';
}

static void
purger(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
    {}
}

static void
clean (char *string)
{
    char *p = strchr(string, '\n');

    if (p)
    {
        *p = 0;
    }

    else
    {
        purger();
    }
}

move_t
human_player (board_t *board, size_t param)
{
  char *boardstr = board_str (board);
  score_t score = board_score (board);
  size_t size = board_get_size (board);
  size_t sizestr = (2 + size * 2) * (size + 1);
  char s[20] = {0};
  FILE *fd;
  move_t move;
  for (size_t i = 0; i < sizestr; i++)
    {
      printf ("%c", boardstr[i]);
    }
  free(boardstr);
  printf ("\nScore: 'X' = %lu, 'O' = %lu\n", score.black, score.white);
  printf ("\n'%c' player's turn.\n", board_get_player (board));

  while (1)
  {
    printf("\nGive your move (e.g. 'A5' or 'a5'), press 'q' or 'Q' to quit: ");
    if (fgets(s, 3, stdin))
    {
      clean(s);
        if (isalpha(s[0]) && isdigit(s[1]) && s[2] == '\0')
        {
          size_t row = toupper (s[0]);
          size_t column = s[1];
          move.row = row - 'A';
          move.column = column - '0';
          if (board_is_move_valid (board, move))
          {
            return move;
          }
          else if ((row - 'A') >= size)
          {
            printf("Row out of bounds. Wrong input, try again!\n");
            continue;
          }
          else if ((column - '0') >= size)
          {
            printf("Column out of bounds. Wrong input, try again!\n");
            continue;
          }
          else if (!board_is_move_valid (board, move))
          {
            printf("This move is invalid. Wrong input, try again!\n");
            continue;
          }
        }
        else if ((s[0] == 'q' || s[0] == 'Q') && (s[1] == '\0'))
        {
            printf("\nQuitting, do you want to save this game (y/N)?");
            if(fgets(s, 2, stdin))
            {
                clean(s);
                if (s[0] == 'y' || s[0] == 'Y')
                {
                    printf("\nGive a filename to save the game (default:"
                           " 'board.txt'):");
                    if (fgets(s, 12, stdin))
                    {
                      clean(s);
                      if (s != NULL )
                      {
                        removeSpaces(s);
                        fd = fopen(s, "w");
                        board_save(board, fd);
                        fclose(fd);
                      }
                      else
                      {
                        fd = fopen("board.txt", "w");
                        board_save(board, fd);
                        fclose(fd);
                      }
                    move.row = size;
                    move.column = size;
                    board_set_player(board, EMPTY_STONE);
                    return move;
                }
               }
               else
               {
                 move.row = size;
                 move.column = size;
                 board_set_player (board, EMPTY_STONE);
                 return move;
               }
         }
        }
        else if (!isalpha(s[0]) || !isdigit(s[1]))
        {
            printf("Wrong input, try again!\n");
            continue;
        }
    }
  }
}

static int score_heuristic (board_t *board, stone_t player)
{
  score_t score = board_score (board);
  int score_heuristic = score.white - score.black;
  if (player == BLACK_STONE)
    score_heuristic = score.black - score.white;
  return score_heuristic;
}

static int parity_heuristic (board_t *board, stone_t player)
{
  int count, oppositeCount;
  float coinParity = 0;

  score_t score = board_score (board);
  if (player == BLACK_STONE )
    count = score.black;
    oppositeCount = score.white;
  if (player == BLACK_STONE )
    count = score.white;
    oppositeCount = score.white;
  if( count > oppositeCount )
		 coinParity = ( 100 * count ) / ( count + oppositeCount );
	else if( count < oppositeCount )
		 coinParity = -( 100 * oppositeCount ) / ( count + oppositeCount );
  return coinParity;
}

static int mobility_heuristic (board_t *board, stone_t player)
{
  size_t count, oppositeCount;
  float mobility = 0;

  moves_t *moves = board_get_moves(board);
  count = moves_count(board);
  board_set_player(board, get_opponent_player(board));
  moves = board_get_moves(board);
  oppositeCount = moves_count(board);
  board_set_player(board, player);
	if( count > oppositeCount )
		mobility = ( 100 * count ) / ( count + oppositeCount );
	else if( count < oppositeCount )
		mobility = -( 100 * oppositeCount ) / ( count + oppositeCount );
  return mobility;
}

static int corners_heuristic (board_t *board, stone_t player)
{
  size_t size = board_get_size(board);
  int corner_row [4] = { 0, 0, size - 1, size - 1 };
  int corner_column [4] = { 0, size - 1 , 0, size - 1 };
  int blackCorners = 0;
  int whiteCorners = 0;
  float corners = 0;

  for (size_t i = 0; i < 4; i++)
  {
      if (board_get(board, corner_row[i], corner_column[i]) == BLACK_STONE)
      {
          blackCorners++;
      }
      else if (board_get(board, corner_row[i], corner_column[i]) == WHITE_STONE)
      {
          whiteCorners++;
      }
  }

  if (player == BLACK_STONE)
  {
      corners = 100 * (blackCorners - whiteCorners)
                / (blackCorners + whiteCorners + 1);
  }
  else
  {
      corners = 100 * (whiteCorners - blackCorners)
                / (blackCorners + whiteCorners + 1);
  }
  return corners;
}

stone_t get_opponent_player (board_t *board)
{
  if (board_get_player(board) == BLACK_STONE)
    return WHITE_STONE;
  else
    return BLACK_STONE;
}

static int get_max (stone_t player, board_t *board, size_t depth)
{
  int bestscore = 0;
  int score = -INFINITY;
  if (depth == 0)
    return score_heuristic (board, player);
  moves_t *moves = board_get_moves(board);
  if (!moves)
    return score;
  for (size_t i = 0; i < moves_count(moves); i++)
  {
    board_t * boardcp = board_copy (board);
    board_play (boardcp, moves_get(moves,i));
    if (board_get_player(boardcp) == EMPTY_STONE)
       bestscore = score_heuristic(boardcp , board_get_player(boardcp));
    if (board_get_player(boardcp) == board_get_player(board))
       bestscore = get_max(player, boardcp, depth - 1);
    if (board_get_player(boardcp) == get_opponent_player(board))
       bestscore = get_min(player, boardcp, depth - 1);
    if (bestscore > score)
       score = bestscore;
    board_free (boardcp);
  }
  return score;
}

static int get_min (stone_t player, board_t *board, size_t depth)
{
  int bestscore = 0;
  int score = +INFINITY;
  if (depth == 0)
    return score_heuristic (board, player);
  moves_t *moves = board_get_moves(board);
  if (!moves)
    return score;
  for (size_t i = 0; i <= moves_count(moves); i++)
  {
    board_t * boardcp = board_copy (board);
    board_play (boardcp, moves_get(moves, i));
    if (board_get_player(boardcp) == EMPTY_STONE)
       bestscore = score_heuristic(boardcp, player);
    if (board_get_player(boardcp) == board_get_player(board))
       bestscore = get_min(player, boardcp, depth - 1);
    if (board_get_player(boardcp) == get_opponent_player(board))
       bestscore = get_max(player, boardcp, depth - 1);
    if (bestscore < score)
       score = bestscore;
     board_free (boardcp);
  }
  return score;
}

move_t minimax_player(board_t *board, size_t depth)
{
  move_t bestmove;
  int bestscore = 0;
  int score = -INFINITY;
  moves_t *moves = board_get_moves(board);
  if (!moves)
     return bestmove;
  if (moves_count(moves) == 1)
    {
      bestmove = moves_get(moves, 0);
      return bestmove;
    }
  for (size_t i = 0; i < moves_count(moves); i++)
  {
      board_t * boardcp = board_copy (board);
      board_play (boardcp, moves_get(moves, i));
      bestscore = get_max(board_get_player(board), boardcp, depth - 1);
      if (bestscore > score)
      {
          score = bestscore;
          bestmove = moves_get(moves, i);
      }
     board_free (boardcp);
  }
  return bestmove;
}

int get_max_ab(stone_t player, board_t *board, int alpha, int beta, size_t depth)
{
  int bestscore = 0;
  if (depth == 0)
    return score_heuristic (board, player);
  moves_t *moves = board_get_moves(board);
  if (!moves)
    return alpha;

  for (size_t i = 0; i < moves_count(moves); i++)
  {
    board_t *boardcp = board_copy (board);
    board_play (boardcp, moves_get(moves, i));
    if (board_get_player(boardcp) == EMPTY_STONE)
        bestscore = score_heuristic(boardcp, player);
    if (board_get_player(boardcp) == board_get_player(board))
        bestscore = get_max_ab(player, boardcp,alpha, beta, depth - 1);
    if (board_get_player(boardcp) == get_opponent_player(board))
        bestscore = get_min_ab(player, boardcp, alpha, beta, depth - 1);
    if (bestscore > alpha)
        alpha = bestscore;
    if (alpha >= beta)
    {
      board_free(boardcp);
      break;
    }
    board_free(boardcp);
  }
  return alpha;
}

int get_min_ab(stone_t player, board_t *board, int alpha, int beta, size_t depth)
{
  int bestscore = 0;
  if (depth == 0)
    return score_heuristic (board, player);
  moves_t *moves = board_get_moves(board);
  for (size_t i = 0; i < moves_count(moves); i++)
  {
    board_t * boardcp = board_copy (board);
    board_play (boardcp, moves_get(moves, i));
    if (board_get_player(boardcp) == EMPTY_STONE)
        bestscore = score_heuristic(boardcp, player);
    if (board_get_player(boardcp) == board_get_player(board))
        bestscore = get_min_ab(player, boardcp,alpha, beta, depth - 1);
    if (board_get_player(boardcp) == get_opponent_player(board))
        bestscore = get_max_ab(player, boardcp, alpha, beta, depth - 1);
    if (bestscore < beta)
        beta = bestscore;
    if (alpha <= beta)
    {
      board_free(boardcp);
      break;
    }
    board_free(boardcp);
  }
  return beta;
}

move_t minimax_ab_player(board_t *board, size_t depth)
{
  move_t bestmove;
  int bestscore = 0;
  int alpha = -INFINITY;
  int beta = +INFINITY;
  moves_t *moves = board_get_moves(board);
  if (!moves)
    return bestmove;
  if (moves_count(moves) == 1)
    {
      bestmove = moves_get(moves, 0);
      return bestmove;
    }
  for (size_t i = 0; i < moves_count(moves); i++)
  {
      board_t * boardcp = board_copy (board);
      board_play (boardcp, moves_get(moves, i));
      bestscore = get_max_ab(board_get_player(board), boardcp,  alpha, beta,
                                depth - 1);
      if (alpha < bestscore)
      {
        alpha = bestscore;
        bestmove = moves_get(moves, i);
      }
      board_free(boardcp);
  }
  return bestmove;
}


static int get_negascout ( board_t *board, int alpha, int beta, size_t depth)
{
  int score = 0;
  int bestscore = 0;
  if (depth == 0)
    return score_heuristic (board,board_get_player(board));
  moves_t *moves = board_get_moves(board);
  if (!moves)
     return alpha;

  board_t * boardcp = board_copy (board);
  board_play (boardcp, moves_get(moves, 0));
  score = -get_negascout(boardcp, -beta, -alpha, depth - 1);
  board_free(boardcp);
   if (score >= alpha)
     alpha = score;
   if (score < beta)
     {
       for (size_t i = 0; i < moves_count(moves); i++)
       {
           board_t * boardcp = board_copy (board);
           board_play (boardcp, moves_get(moves, i));
           if (board_get_player(boardcp) == EMPTY_STONE)
               bestscore = score_heuristic(boardcp, board_get_player(boardcp));
           bestscore = -get_negascout(boardcp, -(alpha + 1), -alpha, depth - 1);
           if (bestscore > alpha && bestscore < beta)
               bestscore = -get_negascout(boardcp, -beta, -alpha, depth - 1);
           if (bestscore >= score)
             {
               score = bestscore;
               if (bestscore >= alpha)
                 {
                    alpha = bestscore;
                    if(bestscore >= beta)
                      {
                        board_free(boardcp);
                        break;
                      }
                 }
             }
           board_free(boardcp);
        }
     }
   return score;
}

move_t negascout_player(board_t *board, size_t depth)
{
  move_t bestmove;
  int bestscore = 0;
  int alpha = -INFINITY;
  int beta = +INFINITY;
  moves_t *moves = board_get_moves(board);
  if (!moves)
    return(bestmove);
  if (moves_count(moves) == 1)
    {
      bestmove = moves_get(moves, 0);
      return bestmove;
    }
  for (size_t i = 0; i < moves_count(moves); i++)
  {
    board_t *boardcp = board_copy (board);
    board_play (boardcp, moves_get(moves, i));
    bestscore = get_negascout(boardcp, alpha, beta, depth - 1);
    if (bestscore >= alpha)
    {
      alpha = bestscore;
      bestmove = moves_get(moves, i);
    }
    board_free(boardcp);
  }
  return bestmove;
}
