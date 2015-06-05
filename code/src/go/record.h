#ifndef RECORD_H
#define RECORD_H

#include <stdint.h>
#include <stdio.h>

/**
* \file
* \brief
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
*/

/**
* \brief Creates a .sgf file to record the game in 
*/
FILE* create_file(int rank);

/**
* \brief writes a move to the .sgf
*/
void write_move(FILE* record, int player, uint8_t x, uint8_t y);

#endif /* RECORD_H */

