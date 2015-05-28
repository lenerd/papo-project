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
* \brief Defines possible player inputs.
*/

typedef enum
{
	p_net = 1,
	p_human = 2,
}player_t;

/**
* \brief Sets up and plays a game; Returns the winner.
*/

color_t play(uint8_t board_size, player_t black, player_t white, uint8_t komi);

/**
* \brief Executes a move and returns feedback if it worked.
*/
int execute_move(board_t* board, uint8_t board_size, uint8_t x, uint8_t y, color_t color);

#endif /* GAME_CONTROLLER_H */
