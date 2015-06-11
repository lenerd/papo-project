#ifndef BOARD_H
#define BOARD_H

/**
 * \file
 * \brief Contains everything required for a go board.
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 * \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
 * \ingroup go
 */


#include <stdbool.h>
#include <stdint.h>

/** \brief Represents an invalid 1D position. */
extern const uint16_t invalid_1d;


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
	ps_marked = 8,
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

    /** \brief Size of the buffer. */
    uint16_t buf_size;

    /** \brief Specifies next player to place a stone. */
    color_t turn;

    /** \brief Number of captured black stones. */
    uint16_t black_captured;

    /** \brief Number of captured white stones. */
    uint16_t white_captured;

    /** \brief Cyclic lists of the groups. */
    uint16_t* group_next;

    /**
     * \brief ID of the group of every stone. Equals smallest 1d position of a
     * stone in that group.
     */
    uint16_t* group_id;

    /**
     * \brief Number of liberties for every group. Only valid for entries in
     * group_id!
     */
    uint16_t* group_liberties;

    /** \brief Helper buffer to mark positions. */
    uint8_t* mark_buf;
} board_t;


// typedef struct
// {
//     uint8_t x;
//     uint8_t y;
// } position_t;


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
pos_state_t board_position_state (const board_t* board, uint8_t x, uint8_t y);

/**
 * \brief Checks if a placement is legal.
 * \pre board != NULL
 */
bool board_legal_placement (const board_t* board, uint8_t x, uint8_t y,
                            color_t color);

/**
 * \brief Checks if placing a stone at given position would be suicide.
 * \param board Board to place one.
 * \param x x coordinate
 * \param x y coordinate
 * \param color Color of the stone to place.
 * \return True if it would be suicide, else false.
 * \pre board != NULL
 * \pre x < board->size
 * \pre y < board->size
 */
bool board_test_suicide (const board_t* board, uint8_t x, uint8_t y,
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
uint16_t board_num_liberties (const board_t* board, uint16_t group);

/**
 * \brief Finds the group the selected stone belongs to and returns its id.
 * \param board Context of operation.
 * \param x x coordinate.
 * \param y y coordinate.
 * \return Group id.
 * \pre board != NULL
 * \pre x < board->size
 * \pre y < board->size
 */
uint16_t board_get_group(const board_t* board, uint8_t x, uint8_t y);

/**
 * \brief Checks if the groups next to a position are to be captured and updates
 * the board accordingly.
 * \param board Context of operation.
 * \param x x coordinate.
 * \param y y coordinate.
 * \pre board != NULL
 * \pre x < board->size
 * \pre y < board->size
 * \post board->white_captured and board->black_captured are update.
 */
void board_capture (board_t* board, uint8_t x, uint8_t y);

/**
 * \brief Removes all stones of a group from the board.
 * \param board Context of operation.
 * \param group Group to be captured.
 * \return Number of stones captured.
 * \pre board != NULL
 * \pre group < board->buf_size
 * \pre group == board->group_id[group]
 * \pre board->buffer[group] != ps_empty
 * \post All positions previously belonging to the group are now empty.
 */
uint16_t board_capture_group (board_t* board, uint16_t group);

/**
* \brief Returns the final score from black's perspective including komi.
*/
int board_score(const board_t* board, uint8_t size, uint8_t komi);

/**
 * \brief Merges all groups of one color connecting with the stone on the given
 * position.
 * \param board Context of operation.
 * \param x x coordinate
 * \param y y coordinate
 * \return Group id of merged group.
 * \pre board != NULL
 * \pre x < board->size
 * \pre y < board->size
 * \pre board->grid[x][y] != ps_empty
 * \post ret != invalid_1d
 */
uint16_t board_join_groups (board_t* board, uint8_t x, uint8_t y);

/**
 * \brief Recalculates the number of liberties of the group containing the given
 * position.
 * \param board Context of operation.
 * \param x x coordinate
 * \param y y coordinate
 * \return Number of liberties of that group.
 * \pre board != NULL
 * \pre x < board->size
 * \pre y < board->size
 * \pre board->grid[x][y] != ps_empty
 * \post board->group_liberties[board->group_id[pos]] is updated (for pos being
 * the 1d position).
 */
uint16_t board_calc_liberties (board_t* board, uint8_t x, uint8_t y);

/**
 * \brief Calculates the 1D position in board->buffer from 2D coordinates.
 * \param board Context of the positions.
 * \param x x coordinate
 * \param y y coordinate
 * \return The 1D coordinate of the given position.
 * \pre board != NULL
 * \pre x < board->size
 * \pre y < board->size
 * \post &board->buffer[ret] == &board->grid[x][y]
 */
uint16_t board_2d_to_1d (const board_t* board, uint8_t x, uint8_t y);

// position_t board_1d_to_2d (const board_t* board, uint16_t z);

/**
 * \brief Calculates the 1D position left of a given position.
 * \param board Context of the positions.
 * \param pos
 * \return Position one to the left of pos or invalid_1d if the calculated
 * positon is invalid.
 * \pre board != NULL
 * \pre pos < board->buf_size
 */
uint16_t board_1d_left (const board_t* board, uint16_t pos);

/**
 * \brief Calculates the 1D position right of a given position.
 * \param board Context of the positions.
 * \param pos
 * \return Position one to the right of pos or invalid_1d if the calculated
 * positon is invalid.
 * \pre board != NULL
 * \pre pos < board->buf_size
 */
uint16_t board_1d_right (const board_t* board, uint16_t pos);

/**
 * \brief Calculates the 1D position top of a given position.
 * \param board Context of the positions.
 * \param pos
 * \return Position one to the top of pos or invalid_1d if the calculated
 * positon is invalid.
 * \pre board != NULL
 * \pre pos < board->buf_size
 */
uint16_t board_1d_top (const board_t* board, uint16_t pos);

/**
 * \brief Calculates the 1D position bottom of a given position.
 * \param board Context of the positions.
 * \param pos
 * \return Position one to the bottom of pos or invalid_1d if the calculated
 * positon is invalid.
 * \pre board != NULL
 * \pre pos < board->buf_size
 */
uint16_t board_1d_bot (const board_t* board, uint16_t pos);

#endif /* BOARD_H */
