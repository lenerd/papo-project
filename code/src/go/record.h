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
FILE* create_file(int);

/**
* \brief writes a move to the .sgf
*/
void write_move(FILE*, int, uint8_t, uint8_t);

#endif /* RECORD_H */

