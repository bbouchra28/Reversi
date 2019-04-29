#ifndef REVERSI_H
#define REVERSI_H

#include "board.h"

#define VERSION 1
#define SUBVERSION 0
#define REVISION 0

board_t *board_load(char *filename);
int game (move_t (*black)(board_t*, size_t), move_t (*white)(board_t*, size_t),
          board_t *board);

#endif /* REVERSI_H */
