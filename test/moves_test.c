#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include <moves.h>

/* gcc -Wall -Wextra -std=c11 -I ../include -c moves_test.c */
/* gcc -Wall -Wextra -std=c11 -o moves_test moves_test.o moves.o */

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
  /* Freeing NULL pointer */
  moves_free(NULL);
  EXPECT("moves_free(NULL)", (true));

   /* Allocate and free memory */
  moves_t *moves_0 = moves_alloc(0);
  EXPECT("moves_alloc(0) == NULL || moves_alloc(0) != NULL", (true));
  moves_free (moves_0);

  moves_t *moves_1 = moves_alloc(1);
  EXPECT("moves_alloc(1) != NULL", (moves_1 != NULL));
  moves_free (moves_1);

  moves_t *moves_10 = moves_alloc(10);
  EXPECT("moves_alloc(10) != NULL", (moves_10 != NULL));
  moves_free (moves_10);

  /* moves_count(), moves_set() and moves_get() */
  const move_t moves_array[10] = {
    { 1, 1 }, { 2, 2 }, { 3, 3 }, { 43, 34 }, { 0, 0 },
    { 42, 42 }, { 9, 9 }, { 8, 8 }, { -1, -1 }, { 4, 4 }
  };

  moves_10 = moves_alloc(10);
  EXPECT("moves_alloc(10) != NULL", (moves_10 != NULL));
  EXPECT("moves_count(moves_alloc(10)) == 10", (moves_count(moves_10) == 10));
  EXPECT("moves_count(NULL) == 0", (moves_count(NULL) == 0));

  for (size_t i = 0; i < moves_count(moves_10); ++i)
    EXPECT("moves_set() == 0", (moves_set(moves_10, i, moves_array[i]) == 0));

  EXPECT("moves_set(moves, 1000, ...) == -1",
	 moves_set(moves_10, 1000, moves_array[0]) == -1);

  EXPECT("moves_set(moves, -1, ...) == -1",
	 moves_set(moves_10, -1, moves_array[0]) == -1);

  EXPECT("moves_set(moves, 10, ...) == -1",
	 moves_set(moves_10, 10, moves_array[0]) == -1);

  for (size_t i = 0; i < moves_count(moves_10); ++i)
    EXPECT("moves_get()",
	   (moves_get(moves_10, i).row == (moves_array[i]).row) &&
	   (moves_get(moves_10, i).column == (moves_array[i]).column));

  EXPECT("moves_get(moves, 1000) == { MAX_SIZE, MAX_SIZE }",
	 moves_get(moves_10, 1000).row == MAX_SIZE &&
	 moves_get(moves_10, 1000).column == MAX_SIZE);

  EXPECT("moves_get(moves, -1) == { MAX_SIZE, MAX_SIZE }",
	 moves_get(moves_10, -1).row == MAX_SIZE &&
	 moves_get(moves_10, -1).column == MAX_SIZE);

  EXPECT("moves_get(moves, 10) == { MAX_SIZE, MAX_SIZE }",
	 moves_get(moves_10, 10).row == MAX_SIZE &&
	 moves_get(moves_10, 10).column == MAX_SIZE);

  /* moves_copy() */
  EXPECT("moves_copy(NULL) == NULL", (moves_copy(NULL) == NULL));

  moves_0 = moves_copy(moves_10);
  EXPECT("moves_copy(moves) != NULL", (moves_0 != NULL));

  for (size_t i = 0; i < moves_count(moves_0); ++i)
    EXPECT("moves_get()",
	   (moves_get(moves_0, i).row == (moves_array[i]).row) &&
	   (moves_get(moves_0, i).column == (moves_array[i]).column));

  /* move2str() and moves2str() */
  for (size_t i = 0; i < moves_count(moves_10); ++i)
    {
      char *str = move2str(moves_get(moves_10, i));
      fprintf(stdout, "Output: '%s' ", str);
      EXPECT("move2str() != NULL", str != NULL);
      free(str);
    }

  char *str = moves2str(moves_10);
  fprintf(stdout, "Output '%s' ", str);
  EXPECT("moves2str() != NULL", str != NULL);
  free(str);

  /* Freeing memory */
  moves_free (moves_0);
  moves_free (moves_10);

  return EXIT_SUCCESS;
}
