#ifndef RECORD_H
#define RECORD_H

#include <stdint.h>
#include <stdio.h>
#include "board.h"

/**
 * \file
 * \brief
 * \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
 */

/**
 * \brief Creates a .sgf file to record the game in
 */
FILE* create_file (const char* file_name);

/**
 * \brief writes a move to the .sgf
 */
void write_move (FILE* record, color_t color, uint8_t x, uint8_t y);

#endif /* RECORD_H */
