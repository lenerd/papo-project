#ifndef BOARD_H
#define BOARD_H

/**
 * \file
 * \brief Contains everything required for a go board.
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 * \ingroup go
 */


#include <stdbool.h>
#include <stdint.h>


/**
 * \brief Defines possible colors of a stone.
 */
typedef enum
{
    c_black = 1,
    c_white = 2,
} color_t;


/**
 * \brief Defines possible states of a position
 */
typedef enum
{
    ps_empty = 0,
    ps_black = 1,
    ps_white = 2,
    ps_illegal = 4,
	ps_marked = 5,
} pos_state_t;

/**
 * \brief Go board.
 */
typedef struct
{
    /** \brief Buffer for actual board. */
    uint8_t* buffer;
    /** \brief 2D interface */
    uint8_t** grid;
    /** \brief Size of the board. */
    uint8_t size;
    /** \brief Specifies next player to place a stone. */
    color_t turn;
    /** \brief Number of captured black stones. */
    uint16_t black_captured;
    /** \brief Number of captured white stones. */
    uint16_t white_captured;
} board_t;


/**
 * \brief Creates a board of given size.
 * \pre size > 0
 */
board_t* board_create (uint8_t size);

/**
 * \brief Destroys a board.
 * \pre board != NULL
 * \post all used memory freed
 */
void board_destroy (board_t* board);

/**
 * \brief Return the state of specified position on the board.
 * \pre board != NULL
 */
pos_state_t board_position_state (board_t* board, uint8_t x, uint8_t y);

/**
 * \brief Checks if a placement is legal.
 * \pre board != NULL
 */
bool board_legal_placement (const board_t* board, uint8_t x, uint8_t y,
                            color_t color);

/**
 * \brief Places a stone on given position.
 * \pre board_legal_placement(board, x, y, color) == true
 * \pre board != NULL
 * \post turn != color
 * \post board_legal_placement(board, x, y, color) == false
 */
void board_place (board_t* board, uint8_t x, uint8_t y);

/**
 * \brief Don't place a stone and change turn.
 * \pre board != NULL
 * \post turn != color
 */
void board_pass (board_t* board);

/**
 * \brief The number of liberties of a (group of) stone(s).
 * \pre board->grid[x][y] == ps_black || board->grid[x][y] == ps_white
 */
uint16_t board_num_liberties (const board_t* board, uint8_t x, uint8_t y);

#endif /* BOARD_H */
