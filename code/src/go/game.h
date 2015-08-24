#ifndef GAME_H
#define GAME_H


/**
 * \file
 * \brief Defines the game class as framework for a round of go.
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 * \ingroup go
 */

#include "player.h"
#include "board.h"

#include <stddef.h>
#include <stdint.h>


/**
 * \brief Represents a game.
 */
typedef struct
{
    /** \brief Player controlling black */
    player_t* black;
    /** \brief Player controlling white */
    player_t* white;

    /** \brief Board to play on. */
    board_t* board;
    /** \brief Whose turn it is. */
    color_t turn;

    /** \brief Whether the game is finished. */
    bool finished;

    /** \brief Whether the last move was passed. */
    bool passed;

    /** \brief Number of moves executed (play_cnt + pass_cnt) */
    uint64_t move_cnt;
    /** \brief Number of plays. */
    uint64_t play_cnt;
    /** \brief Number of passes. */
    uint64_t pass_cnt;
    /** \brief Maximum of moves allowed. */
    uint64_t move_limit;
} game_t;


/**
 * \brief Let two players play a game of go.
 * \param black Player for black.
 * \param white Player for white.
 * \param board_size Size of the board to play on.
 * \param move_limit Maximum number of moves allowed (in total) 0 means
 * unlimited moves.
 * \return
 * \pre black != NULL
 * \pre white != NULL
 * \pre board_size > 1
 */
game_t* game_create (player_t* black, player_t* white, size_t board_size,
                     uint64_t move_limit);

/**
 * \brief Destroys the game struct.
 *
 * Players are not destroyed.
 * \param game
 * \pre game != NULL
 * \post Resources of game and board are freed.
 */
void game_destroy (game_t* game);

/**
 * \brief Makes on move in the given game.
 * \param game
 * \pre game != NULL
 */
void game_step (game_t* game);

/**
 * \brief Calculates the current score of the game.
 * \param game
 * \return score
 * \pre game != NULL
 */
int64_t game_score (const game_t* game);

#endif /* GAME_H */
