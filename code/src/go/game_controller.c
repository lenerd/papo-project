#include "board.h"
#include "record.h"
#include "game_controller.h"
#include "neuralnet/neuralnet.h"
#include <stdio.h>
#include <stdlib.h>

result_t* play(uint8_t board_size, neuralnet_t* black, neuralnet_t* white, uint8_t komi, FILE* record)
{
	board_t* board = board_create(board_size);
	bool game_over = false;
	int8_t passed  = 0;
	result_t* final = result_init(black, white);

	//Game loop
	while(game_over == false)
	{
		move_t* move = genmove(board, final);
			
			//No move left
			if(move->x == -1)
			{
				game_over = true;
				break;
			}
			//Pass
			else if(move->x >= board_size)
			{
				board_pass(board);
				++passed;
				if(passed > 1)
				{
					game_over = true;
					break;
				}
			}
			//Placement
			else
			{
				write_move(record, board->turn, (uint8_t)move->x, (uint8_t)move->y);
				board_place(board, (uint8_t)move->x, (uint8_t)move->y);
				passed = 0;
			}
	}
	
	//Scoring
	int score_val = board_score(board);
	final->score_black = score_val;
	final->score_white = - score_val;

	return final;	
}

move_t* genmove(board_t* board, result_t* result)
{
    float* output;
    uint8_t x1, y1, x2, y2;
    x1 = x2 = y1 = y2 = 0;
    uint16_t count = 0;
    int size = board->size * board->size +1; 
    //Holds x and y positions as well as number of tried illegal moves
    move_t* move = malloc(sizeof(move_t));

	//Get output of neural net
	if(board->turn == c_black)
		output = calculate_output(result->black, board->buffer, UINT8);
	else
		output = calculate_output(result->white, board->buffer, UINT8);


	//If count is equal or bigger than size, the whole board has been checked without finding
	//a legal move
	while(count < size)
	{	
		//Finds two highest ranked placements
 		for(uint16_t i = 0; i < size; ++i)
		{
			if(output[i] > output[x1*board->size+y1])
			{
				x2 = x1;		
				y2 = y1;
				x1 = (uint8_t) i / board->size;
				y1 = (uint8_t) i % board->size;
			}
		}
		
		//Checks if one of them is legal
		if(board_legal_placement(board, x1, y1, board->turn))
		{
			move->x = (int8_t)x1;
			move->y = (int8_t)y1;
			break;
		}
		else if(board_legal_placement(board, x1, y1, board->turn))
		{
			move->x = (int8_t)x2;
			move->y = (int8_t)y2;
			++count;
			break;
		}
		//If not, set their rankings to a very low level and start again
		else
		{
			output[x1*board->size+y1] = - 5000;
			output[x2*board->size+y2] = - 5000;
            ++count;
            ++count;
		}
	}
	
	move->count = count;
	
	//In case no move has been found
	if(count >= size)
	{
		move->x = -1;
		move->y = -1;
		move->count = 0;
	}

	return move;
	
}

result_t* result_init(neuralnet_t* black, neuralnet_t* white)
{
    result_t* result = malloc(sizeof(result_t));
	result->black = black;
	result->white = white;
	
	result->score_black = 0;
	result->score_white = 0;	
    return result;
}
