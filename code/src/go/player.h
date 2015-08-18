#ifndef PLAYER_H
#define PLAYER_H

/**
 * \file
 * \brief Defines the player class and provides implementations for human and
 * artificial players.
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 */

#include "go/board.h"
#include "neuralnet/neuralnet.h"


/**
 * \brief Represents a player in a game.
 */
typedef struct player player_t;

/**
 * \brief Function interface for decision making.
 * \param player Player instance
 * \param board Board to play on.
 * \return Position to move to.
 */
typedef position_t (*move_func)(const player_t* player, const board_t* board);

/**
 * \brief Represents a player in a game.
 */
struct player
{
    /** \brief Whether the player is black or white. */
    color_t color;
    /** \brief Function pointer for the decision making. */
    move_func move;
    /** \brief Can provide some extra context for above function. */
    const void* move_context;
};


/**
 * \brief Creates an artificial player controlled by a neural network.
 * \param net Network controlling the player
 * \return player instance
 * \pre net != NULL
 */
player_t* player_create_net (const neuralnet_t* net);

/**
 * \brief Creates a player controlled via stdin.
 * \return player instance
 */
player_t* player_create_human (void);

/**
 * \brief Sets the color of a player.
 * \param player
 * \param color
 * \pre player != NULL
 * \post player->color = color
 */
void player_set_color (player_t* player, color_t color);

/**
 * \brief Let a player decide on a move.
 *
 * An illegal move should be interpretated as pass.
 * \param player Player whose turn it is.
 * \param board Board to play on.
 * \return Move to make.
 * \pre player != NULL
 * \pre board != NULL
 */
position_t player_move (const player_t* player, const board_t* board);

#endif /* PLAYER_H */
