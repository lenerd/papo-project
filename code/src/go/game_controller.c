#include "board.h"
#include "record.h"
#include "game_controller.h"
#include "neuralnet/neuralnet.h"
#include <stdio.h>

result_t play_nets(uint8_t board_size, struct neuralnet* black, struct neuralnet* white, uint8_t komi, FILE* record)
{
	board_t* board = board_create(board_size);
	bool game_over = false;
        int8_t passed  = 0;
	result_t final = result_init(black, white);

	while(game_over == false)
	{
		if(board->turn == c_black)
		{
			int *move[2];		
			
			calculate_output(black, board, move);	

			if(execute_move(board, board_size, move[0], move[1], c_white) == -1)
				game_over = true;
			else if(execute_move(board, board_size, move[0], move[1], c_white) == 1)
				passed = 1;
			else if(execute_move(board, board_size, move[0], move[1], c_white) == 0)
			{
				passed = 0;
				write_move(record, 1, move[0], move[1]);
			}
		}
		else
		{
			int *move[2];

			// calculate_output(white, (float) board, move);
            // FIXME: function wants a pointer to a float array
            // (nullpointer for compilibility only
			calculate_output(white, (float*) NULL, move);
			
			if(execute_move(board, board_size, move[0], move[1], c_white) == -1)
				game_over = true;
			else if(execute_move(board, board_size, move[0], move[1], c_white) == 1)
				passed = 1;
			else if(execute_move(board, board_size, move[0], move[1], c_white) == 0)
			{				
				passed = 0;
				write_move(record, 2, move[0], move[1]);
			}
		}
		
		if(passed > 1)
			game_over = true;
	}
	
	int score_val = score(board, board_size, komi);
	final.score_black = score_val;
	final.score_white = - score_val;

	return final;
	
	
}

int play_human_vs_net(uint8_t board_size, struct neuralnet* net, color_t human_player, uint8_t komi, FILE* record)
{	
	board_t* board = board_create(board_size);
	bool game_over = false;
	uint8_t passed;

	while(game_over == false)
	{
		if(board->turn == human_player)
		{
			int *move[2];			
			
			/* TODO: Reading from command line (every round)...  Is that even possible? */
			

			if(execute_move(board, board_size, move[0], move[1], c_white) == -1)
				game_over = true;
			else if(execute_move(board, board_size, move[0], move[1], c_white) == 1)
				passed = 1;
			else if(execute_move(board, board_size, move[0], move[1], c_white) == 0)
				passed = 0;
		}
		else
		{
			int *move[2];

			// calculate_output(net, (float) board, move);
            // FIXME: function wants a pointer to a float array
            // (nullpointer for compilibility only
			calculate_output(net, (float*) NULL, move);
			
			if(execute_move(board, board_size, move[0], move[1], c_white) == -1)
				game_over = true;
			else if(execute_move(board, board_size, move[0], move[1], c_white) == 1)
				passed = 1;
			else if(execute_move(board, board_size, move[0], move[1], c_white) == 0)
				passed = 0;
		}
		
		if(passed > 1)
			game_over = true;
	}
	
	return score(board, board_size, komi);
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

result_t result_init(struct neuralnet* black, struct neuralnet* white)
{
    result_t result;
	result.black = black;
	result.white = white;
	
	result.score_black = 0;
	result.score_white = 0;	
    return result;
}
