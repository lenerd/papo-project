#include "board.h"
#include "record.h"
#include "game_controller.h"
#include "neuralnet/neuralnet.h"
#include <stdio.h>

result_t play_nets(uint8_t board_size, neuralnet_t* black, neuralnet_t* white, uint8_t komi, FILE* record)
{
	board_t* board = board_create(board_size);
	bool game_over = false;
    int8_t passed  = 0;
	result_t final = result_init(black, white);
    float move[2];
    int ret;

	while(game_over == false)
	{
		if(board->turn == c_black)
		{
			calculate_output(black, (float*) NULL, move);	
			ret = execute_move(board, (uint8_t) move[0], (uint8_t) move[1], c_white);
			
            switch (ret)
            {
                case -1:
                    game_over = true;
                    break;
                case 1:
                    passed = 1;
                    break;
                case 0:
                    passed = 0;
                    write_move(record, 1, (uint8_t) move[0], (uint8_t) move[1]);
                    break;
                default:
                    break;
			}
		}
		else
		{
			// calculate_output(white, (float) board, move);
            // FIXME: function wants a pointer to a float array
            // (nullpointer for compilibility only
			calculate_output(white, (float*) NULL, move);
            ret = execute_move(board, (uint8_t) move[0], (uint8_t) move[1], c_white);
			
            switch (ret)
            {
                case -1:
                    game_over = true;
                    break;
                case 1:
                    passed = 1;
                    break;
                case 0:
                    passed = 0;
                    write_move(record, 2, (uint8_t) move[0], (uint8_t) move[1]);
                    break;
                default:
                    break;
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

int play_human_vs_net(uint8_t board_size, neuralnet_t* net, color_t human_player, uint8_t komi, FILE* record)
{	
	board_t* board = board_create(board_size);
	bool game_over = false;
	uint8_t passed;
    int ret;

	while(game_over == false)
	{
		if(board->turn == human_player)
		{
            uint8_t move[2];
			/* TODO: Reading from command line (every round)...  Is that even possible? */
			
			ret = execute_move(board, move[0], move[1], c_white);
            switch (ret)
            {
                case -1:
                    game_over = true;
                    break;
                case 1:
                    passed = 1;
                    break;
                case 0:
                    passed = 0;
                    break;
                default:
                    break;
            }
		}
		else
		{
            float move[2];
			// calculate_output(net, (float) board, move);
            // FIXME: function wants a pointer to a float array
            // (nullpointer for compilibility only
			calculate_output(net, (float*) NULL, move);
			ret = execute_move(board, (uint8_t) move[0], (uint8_t) move[1], c_white);
			
            switch (ret)
            {
                case -1:
                    game_over = true;
                    break;
                case 1:
                    passed = 1;
                    break;
                case 0:
                    passed = 0;
                    break;
                default:
                    break;
            }
		}
		
		if(passed > 1)
			game_over = true;
	}
	
	return score(board, board_size, komi);
}


int execute_move(board_t* board, uint8_t x, uint8_t y, color_t color)
{
	if (x > board->size || y > board->size)
	{			
		board_pass(board);
		return 1;
	}
	else if(board_legal_placement(board, x, y, color))
	{
		board_place(board, x, y);
		return 0;
	}	

	return -1;
}

result_t result_init(neuralnet_t* black, neuralnet_t* white)
{
    result_t result;
	result.black = black;
	result.white = white;
	
	result.score_black = 0;
	result.score_white = 0;	
    return result;
}
