#include "reversi.h"
#include "player.h"

#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage (int status)
{
  if (status == EXIT_SUCCESS)
    {
      fprintf(stdout,"Usage: reversi [OPTIONS] [-s SIZE] [FILE]\n"
      "Play a reversi game interactively with humans or AIs.\n"
      "-s, --size SIZE    board size (min=1, max=4 (default))\n"
      "-c : --contest     enable 'contest mode'\n"
      "-b, --black-ai     set black player as an AI\n"
      "-w, --white-ai     set white player as an AI\n"
      "-a, --all-ai       set both players as an AI\n"
      "-h, --help         display this help and exit\n"
      "-v, --verbose      verbose output\n"
      "-V, --version      display version and exit\n");
    }
  else
    {
      fprintf(stderr, "Try 'reversi --help' for more information.\n");
    }
  exit(EXIT_SUCCESS);
}

static void
removeSpaces(char *str)
{
    int count = 0;
    for (int i = 0; str[i]; i++)
    {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\r' && str[i] != '\v'
            && str[i] != '\f' && str[i] != '#' && str[i] != '\n')
            str[count++] = str[i];
        if (str[i] == '#')
           break;
    }
    str[count] = '\0';
}

static char
is_wrong(char *line)
{
  for (size_t i = 0; i < strlen(line); i++)
  {
    if ( line[i] != 'X' && line[i] != 'O' && line[i] != '_')
    {
      return line[i];
    }
  }
  return NULL;
}

static int is_empty(const char *str)
{
    int x = 1;
    while (*str)
    {
        if (isspace(*str++))
        {
            x = 0;
        }
    }
    return x;
}

board_t *
board_load(char *filename)
{
  board_t *board = NULL;
  FILE *fd = fopen(filename, "r");
  char line [ 2048 ] = {0};
  size_t line_number = 0;
  size_t emptyorcomment = 0;
  stone_t player = EMPTY_STONE;
  size_t size = 0;
  if(!fd)
    return NULL;
  while ( fgets ( line, sizeof(line), fd ) )
  {
    if (*line == '#')
    {
      emptyorcomment++;
      continue;
    }
    else if (is_empty(line))
    {
      printf("ligne vide\n");
      emptyorcomment++;
      continue;
    }
    else
    {
      removeSpaces(line);
      if(!is_wrong(line))
      {
          line_number++;
          if (line_number == 1 && (line[0] == 'X' || line[0] == 'O')
          && (strlen(line) == 1))
          {
            player = line[0];
            continue;
          }
          else if (line_number == 1 && (line[0] != 'X' || line[0] != 'O'))
          {
            fprintf (stderr, "reversi: error: incorrect player\n");
            fclose(fd);
            exit(EXIT_FAILURE);
          }
          else if (line_number == 2 && strlen(line) > 1 &&
                   strlen(line) <= 8 && strlen(line) % 2 == 0)
          {
              size = strlen(line);
              board = board_init (size);
              board_set_player(board, player);
              if (!board)
                return NULL;
              goto LOOP;
          }
          else if ((line_number >= 2) && strlen(line) > size)
          {
            fprintf (stderr, "reversi: error: board width is too long at"
             " line %lu\n", line_number);
            board_free(board);
            fclose(fd);
            exit(EXIT_FAILURE);
          }
          else if ((line_number > 2) && ((strlen(line) < size ||
                  strlen(line) % 2 == 0)))
          {
              if (line_number - 1 > size)
              {
                fprintf (stderr, "reversi: error: board has too many rows\n");
                board_free(board);
                fclose(fd);
                exit(EXIT_FAILURE);
              }
              else if (line_number <= size + 1)
              {
                if (strlen(line) < size)
                {
                  fprintf (stderr, "reversi: error: board width is too short at"
                  " line %lu\n", line_number);
                  board_free(board);
                  fclose(fd);
                  exit(EXIT_FAILURE);
                }
                else if (strlen(line) == size)
                {
                  LOOP :
                    for (size_t i = 0; i < size; i++)
                    {
                        board_set(board, line[i], line_number - 2, i);
                    }
                    continue;
                }
              }
          }
          else if (line_number == 2 && (strlen(line) < 1 ||
                   strlen(line) > 8 || strlen(line) % 2 != 0))
          {
            fprintf (stderr, "reversi: error: wrong board size\n");
            fclose(fd);
            exit(EXIT_FAILURE);
          }
      }
      else
      {
        fprintf (stderr, "reversi: error: wrong character '%c' at line %lu\n",
        is_wrong(line), line_number + emptyorcomment + 1);
        board_free(board);
        fclose(fd);
        exit(EXIT_FAILURE);
      }
    }
  }
  fclose(fd);
  if (line_number == 1)
  {
    fprintf (stderr, "reversi: error: no board\n");
    exit(EXIT_FAILURE);
  }
  else if (line_number > 1 && line_number - 1 < size)
  {
    fprintf (stderr, "reversi: error: board has too few rows\n");
    board_free(board);
    exit(EXIT_FAILURE);
  }
  return board;
}

int game (move_t (*black)(board_t*, size_t), move_t (*white)(board_t*, size_t),
          board_t *board)
{
  if (!board)
    return NULL;
  printf("Welcome to this reversi game!\n");
  if (black == human_player)
  {
    printf("Black player (X) is human and ");
  }
  else if (black == random_player || black == minimax_player)
  {
    printf("Black player (X) is AI and ");
  }
  if (white == human_player)
  {
    printf("white player 'O' is human.\n");
  }
  else if (white == random_player || white == minimax_player)
  {
    printf("white player 'O' is AI.\n");
  }

  if (board_get_player(board) == BLACK_STONE)
      printf("Black player starts !\n");
  if (board_get_player(board) == WHITE_STONE)
      printf("White player starts !\n");
  if (board_get_player(board) == EMPTY_STONE)
      printf("Game over !\n");

  size_t size = board_get_size (board);
  score_t score;
  move_t move;
  board_t *boardcp = board_copy(board);
  board_set_player(boardcp, get_opponent_player(boardcp));

  while (moves_count(board_get_moves(board)) +
         moves_count(board_get_moves(boardcp)) > 0)
  {
    if (board_get_player(board) == BLACK_STONE &&
        moves_count(board_get_moves(board)))
    {
      move = (*black)(board, 4);
      if (move.row == size && move.column == size)
      {
        printf("Player 'X' resigned. Player 'O' win the game.\n");
        board_free(boardcp);
        return -1;
      }
      board_play(board, move);
    }
    else if (board_get_player(board) == WHITE_STONE &&
             moves_count(board_get_moves(board)))
    {
      move = (*white)(board, 4);
      if (move.row == size && move.column == size)
      {
        printf("Player 'O' resigned. Player 'X' win the game.\n");
        board_free(boardcp);
        return -2;
      }
      board_play(board, move);
    }
    else if (board_get_player(board) == BLACK_STONE &&
             !moves_count(board_get_moves(board)))
    {
      printf("Player 'X' have no valid moves. Pass turn to Player 'O'.\n");
    }
    else if (board_get_player(board) == WHITE_STONE &&
             !moves_count(board_get_moves(board)))
    {
      printf("Player 'O' have no valid moves. Pass turn to Player 'X'.\n");
    }
    else
    {
      goto LOOP;
    }
    board_set_player(boardcp, get_opponent_player(boardcp));
  }
  LOOP :
  board_free(boardcp);
  score = board_score(board);
  if (score.black > score.white)
  {
    fputs("\n", stdout);
    board_print(board, stdout);
    fputs("\n", stdout);
    printf("Player 'X' win the game.\n");
    return 1;
  }
  else if (score.black < score.white)
  {
    fputs("\n", stdout);
    board_print(board, stdout);
    fputs("\n", stdout);
    printf("Player 'O' win the game.\n");
    return 2;
  }
  else
  {
    fputs("\n", stdout);
    board_print(board, stdout);
    fputs("\n", stdout);
    printf("Draw game, no winner.\n");
    return 0;
  }
  printf("Thanks for playing, see you soon!\n");
}

int main(int argc, char* argv[])
{
  int optc;
  static bool verbose = false;
  size_t board_size = 8;
  int game_mode = 0;
  FILE *inputfile = NULL;
  static struct option long_opts[] =
    {
      {"size SIZE", required_argument, NULL, 's'},
      {"contest", no_argument, NULL, 'c'},
      {"black-ai", no_argument, NULL, 'b'},
      {"white-ai", no_argument, NULL, 'w'},
      {"all-ai", no_argument, NULL, 'a'},
      {"help", no_argument, NULL, 'h'},
      {"verbose", no_argument, NULL, 'v'},
      {"version", no_argument, NULL, 'V'},
      {NULL, 0, NULL, 0}
    };

  while ((optc = getopt_long (argc, argv, "s:cbwahvV", long_opts, NULL)) != -1)
      switch (optc) {
        case 's':
            board_size = atoi(optarg) * 2;
            if (board_size > 8 || board_size < 2)
              {
               fprintf(stderr, "The board size must be between 1 and 4.\n");
               usage (EXIT_FAILURE);
              }
            else {
              printf("The size of the board is %lu.\n", board_size);
            }
            break;
        case 'c':
            if (argc < 3)
              {
                fprintf(stderr, "reversi: no input file given.\n");
                usage(EXIT_FAILURE);
              }
            inputfile = fopen (argv[2], "r");
            if (!inputfile)
              {
                perror("reversi");
                exit(EXIT_FAILURE);
              }
            printf("Contest mode enabled.\n");
            break;
        case 'b':
            game_mode = 1;
            printf("Black player is an AI.\n");
            break;
        case 'w':
            game_mode = 2;
            printf("White player is an AI.\n");
            break;
        case 'a':
            game_mode = 3;
            printf("All players are an AIs.\n");
            break;
        case 'h':
            usage(EXIT_SUCCESS);
            break;
        case 'v':
            verbose = true;
            printf("Verbose = true.\n");
            break;
        case 'V':
            printf("reversi 1.0.0\nThis software allows to play to reversi"
                   " game.\n");
            break;
        default:
            usage (EXIT_FAILURE);
  }

    board_t *board = board_init(board_size);
    switch (game_mode)
    {
      case 0:
        game (human_player, human_player, board);
        break;
      case 1:
        //game (minimax_player, human_player, board);
        //game (minimax_ab_player, human_player, board);
        game (negascout_player, human_player, board);
        //game (random_player, human_player, board);
        break;
      case 2:
        //game (human_player, minimax_player, board);
        //game (human_player, minimax_ab_player, board);
        game (human_player, negascout_player, board);
        //game (human_player, random_player, board);
        break;
      case 3:
        //game (negascout_player, negascout_player, board);
        //game (negascout_player, minimax_player, board);
        game (negascout_player, minimax_ab_player, board);
        //game (negascout_player, random_player, board);
        //game (minimax_ab_player, minimax_ab_player, board);
        game (minimax_ab_player, minimax_player, board);
        //game (minimax_ab_player, negascout_player, board);
        //game (minimax_ab_player, random_player, board);
        //game (minimax_player, minimax_player, board);
        //game (minimax_player, minimax_ab_player, board);
        //game (minimax_player, negascout_player, board);
        //game (minimax_player, random_player, board);
        //game (random_player, random_player, board);
        //game (random_player, minimax_player, board);
        //game (random_player, minimax_ab_player, board);
        //game (random_player, negascout_player, board);
        break;
      default:
        fprintf(stdout, "reversi : error incorrect game mode.\n");
        break;
    }
    board_free(board);

}
