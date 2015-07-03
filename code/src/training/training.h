#ifndef TRAINING_H
#define TRAINING_H

/**
* \file
* \brief Contains everything required for neural networks.
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
* \ingroup training
*/

/**
* \brief Executes a backpropagation algorithm to train the nets.
*/
float* backpropagation(neuralnet_t* net, int board_size, float threshold, uint8_t** data, uint8_t data_size, float* wanted);

/**
* \brief Reads all .sgf files for the specified board size and returns them as one data set
*/
int** generate_data(int size);

/**
* \brief Converts .sgf to int*
*/
int* read_file(FILE* fp, int size);

#endif /* TRAINING_H */
