#ifndef PLAYER_H
#define PLAYER_H

/**
 * \file
 * foo bar
 */

#include "go/board.h"
#include "neuralnet/neuralnet.h"


typedef struct player player_t;
typedef position_t (*move_func)(const player_t*, const board_t*);

struct player
{
    color_t color;
    move_func move;
    const void* move_context;
};


player_t* player_create_net (const neuralnet_t* net);
player_t* player_create_human (void);

void player_set_color (player_t* player, color_t color);

position_t player_move (const player_t* player, const board_t* board);

#endif /* PLAYER_H */
