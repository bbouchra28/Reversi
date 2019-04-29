#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include <board.h>

/* gcc -Wall -Wextra -std=c11 -I ../include -c board_test.c */
/* gcc -Wall -Wextra -std=c11 -o board_test board_test.o board.o */

void
EXPECT (char *msg, bool test)
{
  if (test)
    fprintf (stdout, "Checking '%s': (passed)\n", msg);
  else
    fprintf (stdout, "Checking '%s': (failed!)\n", msg);
}

void
ASSERT (char *msg, bool test)
{
  if (test)
    fprintf (stdout, "Checking '%s': (passed)\n", msg);
  else
    {
      fprintf (stdout, "Checking '%s': (critical fail! aborting...)\n", msg);
      exit (EXIT_FAILURE);
    }
}

int
main (void)
{
  /* Freeing a NULL pointer */
  board_free(NULL);
  EXPECT("board_free(NULL)", (true));

  /* Allocate and free memory, get_size() and get_player() */
  board_t *board_0 = board_init(0);
  EXPECT("board_init(0) == NULL", (board_0 == NULL));

  board_0 = board_init(1);
  EXPECT("board_init(1) == NULL", (board_0 == NULL));

  board_t *board_1 = board_init(2);
  ASSERT("board_init(2) != NULL", (board_1 != NULL));
  EXPECT("board_get_size() == 2", board_get_size(board_1) == 2);
  /* No move is possible, therefore we need to get EMPTY_STONE */
  EXPECT("board_get_player() == EMPTY_STONE",
	 board_get_player(board_1) == EMPTY_STONE);
  board_free (board_1);

  board_0 = board_init(3);
  EXPECT("board_init(3) == NULL", (board_0 == NULL));

  board_t *board_2 = board_init(4);
  ASSERT("board_init(4) != NULL", (board_2 != NULL));
  EXPECT("board_get_size() == 4", board_get_size(board_2) == 4);
  EXPECT("board_get_player() == BLACK_STONE",
	 board_get_player(board_2) == BLACK_STONE);
  board_free (board_2);

  board_0 = board_init(5);
  EXPECT("board_init(5) == NULL", (board_0 == NULL));

  board_t *board_3 = board_init(6);
  ASSERT("board_init(6) != NULL", (board_3 != NULL));
  EXPECT("board_get_size() == 6", board_get_size(board_3) == 6);
  EXPECT("board_get_player() == BLACK_STONE",
	 board_get_player(board_3) == BLACK_STONE);
  board_free (board_3);

  board_0 = board_init(7);
  EXPECT("board_init(7) == NULL", (board_0 == NULL));

  board_t *board_4 = board_init(8);
  ASSERT("board_init(8) != NULL", (board_4 != NULL));
  EXPECT("board_get_size() == 8", board_get_size(board_4) == 8);
  EXPECT("board_get_player() == BLACK_STONE",
	 board_get_player(board_4) == BLACK_STONE);
  board_free (board_4);

  board_0 = board_init(9);
  EXPECT("board_init(9) == NULL", (board_0 == NULL));

  board_0 = board_init(10);
  EXPECT("board_init(10) == NULL", (board_0 == NULL));

  /* Get and set a cell */
  board_2 =  board_init(4);

  EXPECT("board_get(board_init(4), 0, 0) == EMPTY_STONE",
	 (board_get(board_2, 0, 0) == EMPTY_STONE));

  EXPECT("board_get(board_init(4), 1, 1) == WHITE_STONE",
	 (board_get(board_2, 1, 1) == WHITE_STONE));

  EXPECT("board_get(board_init(4), 1, 2) == BLACK_STONE",
	 (board_get(board_2, 1, 2) == BLACK_STONE));

  EXPECT("board_get(board_init(4), 4, 2) == EMPTY_STONE",
	 (board_get(board_2, 4, 2) == EMPTY_STONE));

  EXPECT("board_get(board_init(4), 42, 42) == EMPTY_STONE",
	 (board_get(board_2, 42, 42) == EMPTY_STONE));

  EXPECT("board_set(board_init(4), BLACK_STONE, 0, 0)",
	 (board_set(board_2, BLACK_STONE, 0, 0) == 0 &&
	  board_get(board_2, 0, 0) == BLACK_STONE));

  EXPECT("board_set(board_init(4), WHITE_STONE, 3, 3)",
	 (board_set(board_2, WHITE_STONE, 3, 3) == 0 &&
	  board_get(board_2, 3, 3) == WHITE_STONE));

  EXPECT("board_set(board_init(4), EMPTY_STONE, 1, 1)",
	 (board_set(board_2, EMPTY_STONE, 1, 1) == 0 &&
	  board_get(board_2, 1, 1) == EMPTY_STONE));

  EXPECT("board_set(board_init(4), EMPTY_STONE, 42, 42)",
	 (board_set(board_2, EMPTY_STONE, 42, 42) == -1));

  EXPECT("board_set(board_init(4), EMPTY_STONE, -42, 42)",
	 (board_set(board_2, EMPTY_STONE, -42, 42) == -1));

  board_free(board_2);

  EXPECT("board_get(NULL, 0, 0) == EMPTY_STONE",
	 (board_get(NULL, 0, 0) == EMPTY_STONE));

  EXPECT("board_set(NULL, EMPTY_STONE, 0, 0) == -1",
	 (board_set(NULL, EMPTY_STONE, 0, 0) == -1));


  /* Copy a board */
  board_2 = board_init(4);
  board_set(board_2, WHITE_STONE, 0, 0);
  board_set(board_2, BLACK_STONE, 3, 3);
  board_set(board_2, BLACK_STONE, 2, 1); /* Trying non-symetric cell */

  board_0 = board_copy(board_2);

  bool is_same = true;
  for (size_t i = 0; i < board_get_size(board_2); ++i)
    for (size_t j = 0; j < board_get_size(board_2); ++j)
      is_same &= (board_get(board_0, i, j) == board_get(board_2, i, j));
  EXPECT("board == board_copy(board)", is_same);

  board_free(board_0);
  board_free(board_2);

  /* Copy a NULL board */
  EXPECT("board_copy(NULL) == NULL",
	 (board_copy(NULL) == NULL));

  /* Score a board */
  board_2 = board_init(4);
  score_t score = board_score(board_2);
  EXPECT("board_score(board_init(4)) == { 2, 2 }",
	 (score.black == 2 && score.white == 2));

  /* Score empty board */
  board_set(board_2, EMPTY_STONE, 1, 1);
  board_set(board_2, EMPTY_STONE, 1, 2);
  board_set(board_2, EMPTY_STONE, 2, 1);
  board_set(board_2, EMPTY_STONE, 2, 2);
  score = board_score(board_2);
  EXPECT("board_score(board_empty) == { 0, 0 }",
	 (score.black == 0 && score.white == 0));

  /* Score full boards (black and white) */
  for (size_t i = 0; i < board_get_size(board_2); ++i)
    for (size_t j = 0; j < board_get_size(board_2); ++j)
      board_set(board_2, BLACK_STONE, i, j);

  score = board_score(board_2);
  EXPECT("board_score(board_full_black) == { 16, 0 }",
	 (score.black == 16 && score.white == 0));

  for (size_t i = 0; i < board_get_size(board_2); ++i)
    for (size_t j = 0; j < board_get_size(board_2); ++j)
      board_set(board_2, WHITE_STONE, i, j);

  score = board_score(board_2);
  EXPECT("board_score(board_full_white) == { 0, 16 }",
	 (score.black == 0 && score.white == 16));

  board_free(board_2);

  board_4 = board_init(8);
  for (size_t i = 0; i < board_get_size(board_4); ++i)
    for (size_t j = 0; j < board_get_size(board_4); ++j)
      board_set(board_4, WHITE_STONE, i, j);

  score = board_score(board_4);
  EXPECT("board_score(board_full_white) == { 0, 64 }",
	 (score.black == 0 && score.white == 64));

  board_free(board_4);

  /* Board display (string and stdout) */
  char *str = NULL;

  const char *board_str_2 =
    "   0 1\n"
    "A  O X\n"
    "B  X O";

  const char *board_str_4 =
    "   0 1 2 3\n"
    "A  _ * _ _\n"
    "B  * O X _\n"
    "C  _ X O *\n"
    "D  _ _ * _";

  const char *board_str_6 =
    "   0 1 2 3 4 5\n"
    "A  _ _ _ _ _ _\n"
    "B  _ _ * _ _ _\n"
    "C  _ * O X _ _\n"
    "D  _ _ X O * _\n"
    "E  _ _ _ * _ _\n"
    "F  _ _ _ _ _ _";

  const char *board_str_8 =
    "   0 1 2 3 4 5 6 7\n"
    "A  _ _ _ _ _ _ _ _\n"
    "B  _ _ _ _ _ _ _ _\n"
    "C  _ _ _ * _ _ _ _\n"
    "D  _ _ * O X _ _ _\n"
    "E  _ _ _ X O * _ _\n"
    "F  _ _ _ _ * _ _ _\n"
    "G  _ _ _ _ _ _ _ _\n"
    "H  _ _ _ _ _ _ _ _";

  EXPECT("board_str(NULL) == NULL", (board_str(NULL) == NULL));

  board_1 = board_init(2);
  str = board_str(board_1);
  EXPECT("board_str(board_init(2))",
	 (strncmp (str, board_str_2, strlen(board_str_2)) >= 0));
  free(str);
  board_free(board_1);

  board_2 = board_init(4);
  str = board_str(board_2);
  EXPECT("board_str(board_init(4))",
	 (strncmp (str, board_str_4, strlen(board_str_4)) >= 0));
  free(str);
  board_free(board_2);

  board_3 = board_init(6);
  str = board_str(board_3);
  EXPECT("board_str(board_init(6))",
	 (strncmp (str, board_str_6, strlen(board_str_6)) >= 0));
  free(str);
  board_free(board_3);

  board_4 = board_init(8);
  str = board_str(board_4);
  EXPECT("board_str(board_init(8))",
	 (strncmp (str, board_str_8, strlen(board_str_8)) >= 0));
  free(str);
  board_free(board_4);

  /* Checking board_print() */
  /* To uncover next year (2018)... */
  // board_print(NULL, stdout);
  // EXPECT("board_print(NULL, stdout)", (true));

  const char *filename = "test.txt";
  FILE *fd = fopen(filename, "w");
  board_2 = board_init(4);

  board_print(board_2, fd);

  fclose(fd);
  EXPECT("board_print(board_init(4))", !remove(filename));
  board_free(board_2);

  /* Checking board_is_move_valid() */
  board_4 = board_init(8);
  board_set(board_4, WHITE_STONE, 4, 2);
  board_set(board_4, WHITE_STONE, 4, 3);
  board_set(board_4, WHITE_STONE, 4, 4);
  board_set(board_4, BLACK_STONE, 2, 3);
  board_set(board_4, BLACK_STONE, 3, 3);
  board_set(board_4, BLACK_STONE, 3, 4);

  fputs("\n", stdout);
  board_print (board_4, stdout);
  fputs("\n", stdout);

  EXPECT("board_is_move_valid(5,0) == false",
	 !board_is_move_valid(board_4, (move_t) { 5, 0 }));

  EXPECT("board_is_move_valid(5,1) == true",
	 board_is_move_valid(board_4, (move_t) { 5, 1 }));

  EXPECT("board_is_move_valid(5,2) == true",
	 board_is_move_valid(board_4, (move_t) { 5, 2 }));

  EXPECT("board_is_move_valid(5,3) == true",
	 board_is_move_valid(board_4, (move_t) { 5, 3 }));

  EXPECT("board_is_move_valid(5,4) == true",
	 board_is_move_valid(board_4, (move_t) { 5, 4 }));

  EXPECT("board_is_move_valid(5,5) == true",
	 board_is_move_valid(board_4, (move_t) { 5, 5 }));

  EXPECT("board_is_move_valid(5,6) == false",
	 !board_is_move_valid(board_4, (move_t) { 5, 6 }));

  EXPECT("board_is_move_valid(5,7) == false",
	 !board_is_move_valid(board_4, (move_t) { 5, 7 }));

  EXPECT("board_is_move_valid(42,42) == false",
	 !board_is_move_valid(board_4, (move_t) { 42, 42 }));

  EXPECT("board_is_move_valid(-42,42) == false",
	 !board_is_move_valid(board_4, (move_t) { -42, 42 }));

  board_free(board_4);

  /* Checking board_play() */
  board_4 = board_init(8);
  board_set(board_4, WHITE_STONE, 4, 2);
  board_set(board_4, WHITE_STONE, 4, 3);
  board_set(board_4, WHITE_STONE, 4, 4);
  board_set(board_4, BLACK_STONE, 2, 3);
  board_set(board_4, BLACK_STONE, 3, 3);
  board_set(board_4, BLACK_STONE, 3, 4);

  fputs("\n", stdout);
  board_print (board_4, stdout);
  fputs("\n", stdout);

  EXPECT("board_play(5,5) == true",
	 board_play(board_4, (move_t) { 5, 5 }));

  EXPECT("board_get(5,5) == BLACK_STONE",
	 (board_get(board_4, 5, 5) == BLACK_STONE));

  EXPECT("board_get(4,4) == BLACK_STONE",
	 (board_get(board_4, 4, 4) == BLACK_STONE));

  fputs("\n", stdout);
  board_print (board_4, stdout);
  fputs("\n", stdout);

  EXPECT("board_play(1,3) == true",
	 board_play(board_4, (move_t) { 1, 3 }));

  EXPECT("board_get(1,3) == WHITE_STONE",
	 (board_get(board_4, 1, 3) == WHITE_STONE));

  EXPECT("board_get(2,3) == WHITE_STONE",
	 (board_get(board_4, 2, 3) == WHITE_STONE));

  EXPECT("board_get(2,3) == WHITE_STONE",
	 (board_get(board_4, 2, 3) == WHITE_STONE));

  fputs("\n", stdout);
  board_print (board_4, stdout);
  fputs("\n", stdout);

  EXPECT("board_play(3,2) == true",
	 board_play(board_4, (move_t) { 3, 2 }));

  EXPECT("board_get(3,2) == BLACK_STONE",
	 (board_get(board_4, 3, 2) == BLACK_STONE));

  EXPECT("board_get(3,3) == BLACK_STONE",
	 (board_get(board_4, 3, 3) == BLACK_STONE));

  fputs("\n", stdout);
  board_print (board_4, stdout);
  fputs("\n", stdout);

  EXPECT("board_play(4,5) == true",
	 board_play(board_4, (move_t) { 4, 5 }));

  EXPECT("board_get(4,5) == WHITE_STONE",
	 (board_get(board_4, 4, 5) == WHITE_STONE));

  EXPECT("board_get(4,5) == WHITE_STONE",
	 (board_get(board_4, 4, 5) == WHITE_STONE));

  EXPECT("board_get(3,4) == WHITE_STONE",
	 (board_get(board_4, 3, 4) == WHITE_STONE));

  EXPECT("board_get(4,4) == WHITE_STONE",
	 (board_get(board_4, 4, 4) == WHITE_STONE));

  fputs("\n", stdout);
  board_print (board_4, stdout);
  fputs("\n", stdout);

  EXPECT("board_play(0,0) == false",
	 !board_play(board_4, (move_t) { 0, 0 }));

  board_free(board_4);

  board_2 = board_init(4);
  board_set(board_2, BLACK_STONE, 0, 0);
  board_set(board_2, WHITE_STONE, 1, 1);
  board_set(board_2, WHITE_STONE, 2, 2);
  board_set(board_2, EMPTY_STONE, 2, 1);
  board_set(board_2, EMPTY_STONE, 1, 2);

  fputs("\n", stdout);
  board_print (board_2, stdout);
  fputs("\n", stdout);

  EXPECT("board_play(3,3) == true",
	 board_play(board_2, (move_t) { 3, 3 }));
  EXPECT("board_get_player() == EMPTY_STONE",
	 (board_get_player(board_2) == EMPTY_STONE));

  board_free(board_2);

  return EXIT_SUCCESS;
}
