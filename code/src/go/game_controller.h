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

typedef struct
{
    int8_t x;
    int8_t y;
    uint16_t count;
} move_t;

/**
* \brief Sets up and plays a game with two nets.
*/

result_t play(uint8_t board_size, neuralnet_t* black, neuralnet_t* white, uint8_t komi, FILE* record);

/**
* \brief Generates a move from a neuralnet output
*/
move_t genmove(board_t* board, result_t result);

/**
* \brief Initializes the result datatype.
*/
result_t result_init(neuralnet_t* black, neuralnet_t* white);

#endif /* GAME_CONTROLLER_H */
