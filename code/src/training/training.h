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

typedef struct
{
    /* board size */
    int size;
    color_t color;
    board_t* input;
    int* expected;
} training_data_t;

typedef struct
{
    /* length of data */
	int size;
    training_data_t* data;
} dataset_t;

/**
* \brief Reads all .sgf files for the specified board size and returns them as one data set including the expected values for the color
*/
dataset_t* generate_training_data(const char* path, int size, color_t color);

/**
* \brief Generates the expected values for this color. If the position is somehow illegal, expected will be zero, else one.
*/
void generate_expected_values(training_data_t* data);

/**
* \brief Converts .sgf to int*
*/
// TODO: replace with generic load_game function
void input_from_file(training_data_t* data, FILE* fp);

/**
 * Create dataset.
 */
dataset_t* create_dataset (int size);

/**
 * Destroy dataset.
 */
void destroy_dataset (dataset_t* set);

/**
 * Initialize training data.
 */
void create_training_data (training_data_t* data, int size, color_t color);

/**
 * Destroy training data.
 */
void destroy_training_data (training_data_t* data);

#endif /* TRAINING_H */
