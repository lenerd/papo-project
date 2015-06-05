#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H 

/**
* \file
* \brief Provides a frame for two players to play against each other (either nets or humans via command line).
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
* \ingroup go
*/

#include <stdio.h>
#include "board.h"
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

result_t play_nets(uint8_t board_size, neuralnet_t* black, neuralnet_t* white, uint8_t komi, FILE* recordt);

/**
* Let's a human play against a net.
*/

int play_human_vs_net(uint8_t board_size, neuralnet_t* net, color_t human_player, uint8_t komi, FILE* record);

/**
* \brief Executes a move and returns feedback if it worked.
*/
int execute_move(board_t* board, uint8_t x, uint8_t y, color_t color);

/**
* \brief Initializes the result datatype.
*/
result_t result_init(neuralnet_t* black, neuralnet_t* white);

#endif /* GAME_CONTROLLER_H */
