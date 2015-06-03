#include "board.h"
#include "record.h"
#include "game_controller.h"
#include "neuralnet.h"
#include <stdio.h>

struct result play_nets(uint8_t board_size, neuralnet* black, neuralnet* white, uint8_t komi)
{
	board_t* board = board_create(board_size);
	bool game_over = false;

	while(game_over == false)
	{
		if(board->turn == black_c)
		{
			int *move[2];		
			
			calculate_output(black, board, move);	

			if(execute_move(board, board_size, move[0], move[1], white) == -1)
				game_over = true;
			else if(execute_move(board, board_size, move[0], move[1], white) == 1)
				passed = 1;
			else if(execute_move(board, board_size, move[0], move[1], white) == 0)
				passed = 0;
		}
		else
		{
			int *move[2];

			calculate_output(white, (float) board, move);
			
			if(execute_move(board, board_size, move[0], move[1], white) == -1)
				game_over = true;
			else if(execute_move(board, board_size, move[0], move[1], white) == 1)
				passed = 1;
			else if(execute_move(board, board_size, move[0], move[1], white) == 0)
				passed = 0;
		}
		
		if(passed > 1)
			game_over = true;
	}
}

struct result play_human_vs_net(uint8_t board_size, neuralnet* net, color_t human_player, uint8_t komi)
{	
	board_t* board = board_create(board_size);
	bool game_over = false;

	while(game_over == false)
	{
		if(board->turn == human_player)
		{
			int *move[2];			
			
			/* TODO: Reading from command line (every round)...  Is that even possible? */
			

			if(execute_move(board, board_size, move[0], move[1], white) == -1)
				game_over = true;
			else if(execute_move(board, board_size, move[0], move[1], white) == 1)
				passed = 1;
			else if(execute_move(board, board_size, move[0], move[1], white) == 0)
				passed = 0;
		}
		else
		{
			int *move[2];

			calculate_output(net, (float) board, move);
			
			if(execute_move(board, board_size, move[0], move[1], white) == -1)
				game_over = true;
			else if(execute_move(board, board_size, move[0], move[1], white) == 1)
				passed = 1;
			else if(execute_move(board, board_size, move[0], move[1], white) == 0)
				passed = 0;
		}
		
		if(passed > 1)
			game_over = true;
	}
	
	int score = score(board, board_size, komi);

	if(result > 0)
		return c_black;
	else
		return c_white;	
}


int execute_move(board_t* board, uint8_t board_size, uint8_t x, uint8_t y, color_t color)
{
	if( x < 0 || x > board_size || y > 0 || y > board_size)
	{			
		board_pass(board);
		return 1;
	}
	else if(board_legal_placement(board, x, y, color))
	{
		board_place(board, x, y);
		return 0;
	}	
	else
	{
		if(approximate_move(board, x, y, board_size)==-1)
			return -1;
	}

	return 2;
}
