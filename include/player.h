#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"
#define INFINITY 1000;
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void board_save(board_t *board, FILE *fd);
move_t random_player(board_t *board, size_t param);
move_t human_player (board_t *board, size_t param);
int game (move_t (*black)(board_t*, size_t), move_t (*white)(board_t*, size_t),
          board_t *board);
stone_t get_opponent_player (board_t *board);
move_t minimax_player(board_t *board, size_t depth);
move_t minimax_ab_player(board_t *board, size_t depth);
move_t negascout_player(board_t *board, size_t depth);

#endif /* PLAYER_H */
