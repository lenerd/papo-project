#ifndef GAME_H
#define GAME_H

/**
 * \file
 * foobar
 */

#include "player.h"
#include "board.h"

#include <stddef.h>
#include <stdint.h>


typedef struct
{
    player_t* black;
    player_t* white;

    board_t* board;
    int64_t score;
    color_t turn;

    bool finished;

    bool passed;

    uint64_t move_cnt;
    uint64_t move_limit;
} game_t;


game_t* game_create (player_t* black, player_t* white, size_t board_size, uint64_t move_limit);
void game_destroy (game_t* game);

void game_step (game_t* game);

int64_t game_score (game_t* game);

#endif /* GAME_H */
