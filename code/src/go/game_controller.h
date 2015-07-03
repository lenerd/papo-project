#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H 

/**
* \file
* \brief Provides a frame for two players to play against each other (either nets or humans via command line).
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
* \ingroup go
*/

#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "record.h"
#include "neuralnet/neuralnet.h"

/**
* \brief Data type for result: two nets and their changes of fitness.
*/

typedef struct
{
	
	neuralnet_t* black;
	int score_black;

	neuralnet_t* white;
	int score_white;

} result_t;

/**
* \brief Sets up and plays a game with two nets.
*/

result_t play(uint8_t board_size, neuralnet_t* black, neuralnet_t* white, uint8_t komi, FILE* record);

/**
* \brief Generates a move from a neuralnet output
*/
int* genmove(color_t color, result_t result, board_t* board, int board_size);

/**
* \brief Initializes the result datatype.
*/
result_t result_init(neuralnet_t* black, neuralnet_t* white);

#endif /* GAME_CONTROLLER_H */
