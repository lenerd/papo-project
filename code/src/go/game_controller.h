#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H 

/**
* \file
* \brief Provides a frame for two players to play against each other (either nets or humans via command line).
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
* \ingroup go
*/

#include <stdio.h>

/**
* \brief Data type for result: two nets and their changes of fitness.
*/

struct result{
	
	neuralnet* black;
	int score_black;

	neuralnet* white;
	int score_white;

}

/**
* \brief Sets up and plays a game with two nets.
*/

struct result play_nets(uint8_t board_size, neuralnet* black, neuralnet* white, uint8_t komi);

/**
* Let's a human play against a net.
*/

struct result play_human_vs_net(uint8_t board_size, neuralnet* net, colot_t human_player, uint8_t komi);

/**
* \brief Executes a move and returns feedback if it worked.
*/
int execute_move(board_t* board, uint8_t board_size, uint8_t x, uint8_t y, color_t color);

#endif /* GAME_CONTROLLER_H */
