#ifndef TRAINING_H
#define TRAINING_H

/**
* \file
* \brief Contains everything required for neural networks.
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
* \ingroup training
*/

#include "go/board.h"
#include <stdio.h>

struct dataset{
	int dataset_size;

	int** input_values;

	int** expected_values;
}dataset;

/**
* \brief Reads all .sgf files for the specified board size and returns them as one data set including the expected values for the color
*/
struct dataset* generate_data(int size, color_t color);

/**
* \brief Generates the expected values for this color. If the position is somehow illegal, expected will be zero, else one.
*/
int* generate_expected_values(int* positions, int size, color_t color);

/**
* \brief Converts .sgf to int*
*/
int* read_file(FILE* fp, int size);

#endif /* TRAINING_H */
