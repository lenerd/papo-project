#ifndef RECORD_H
#define RECORD_H

#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "game.h"

/**
 * \file
 * \brief
 * \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 */

/**
 * \brief Represents a player in a game.
 */
typedef struct recorder recorder_t;

/**
 * \brief Function interface for decision making.
 * \param rec Recorder instance
 * \param pos Position moved to.
 * \return 0 if success, else != 0
 */
typedef int (*record_func)(recorder_t* rec, position_t pos, bool passed, bool end);

/**
 * \brief Represents a player in a game.
 */
struct recorder
{
    record_func func;
    FILE* file;
    const board_t* board;
};


/**
 * \brief Creates a recorder printing ASCII art to file.
 * \param game Game to record
 * \param file Open, writable file
 * \return new recorder
 * \pre game != NULL
 * \pre file != NULL
 */
recorder_t* recorder_ascii_create (const board_t* board, FILE* file);

/**
 * \brief Creates a recorder printing in SGF to file.
 * \param game Game to record
 * \param file Open, writable file
 * \return new recorder
 * \pre game != NULL
 * \pre file != NULL
 */
recorder_t* recorder_sgf_create (const board_t* board, FILE* file);

/**
 * \brief Destroys a recorder.
 * \param rec
 * \pre rec != NULL
 * \post Ressources are released.
 */
void recorder_destroy (recorder_t* rec);


/**
 * \brief Creates a .sgf file to record the game in
 */
FILE* create_file (const char* file_name);

/**
 * \brief Writes moves into the created file (only use for legal moves without passing!)
 */
void write_move (FILE* record, color_t color, uint8_t x, uint8_t y);

#endif /* RECORD_H */
