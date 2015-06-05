#include "board.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

void board_print (board_t* board, bool complete)
{
    if (complete)
        for (int i = 0; i < board->size + 2; ++i)
        {
            for (int j = 0; j < board->size + 2; ++j)
            {
                printf ("%d ", board->buffer[i * (board->size + 2) + j]);
            }
            printf ("\n");
        }
    else
        for (int i = 0; i < board->size; ++i)
        {
            for (int j = 0; j < board->size; ++j)
            {
                printf ("%d ", board->grid[i][j]);
            }
            printf ("\n");
        }
}

board_t* board_create (uint8_t size)
{
    board_t* board = NULL;

    board = malloc (sizeof (board_t));
    if (board == NULL)
    {
        fprintf (stderr, "memory allocation failed\n");
        return NULL;
    }

    board->buffer = calloc ((size_t)((size + 2) * (size + 2)), sizeof (int8_t));
    if (board->buffer == NULL)
    {
        fprintf (stderr, "memory allocation failed\n");
        free (board);
        return NULL;
    }

    board->grid = calloc ((size_t)(size + 2), sizeof (int8_t*));
    if (board->grid == NULL)
    {
        fprintf (stderr, "memory allocation failed\n");
        free (board);
        free (board->buffer);
        return NULL;
    }

    for (uint8_t i = 0; i < size + 2; ++i)
    {
        board->buffer[i] = ps_illegal;
        board->buffer[(size + 1) * (size + 2) + i] = ps_illegal;
    }
    for (uint8_t i = 1; i < size + 1; ++i)
    {
        board->buffer[i * (size + 2)] = ps_illegal;
        board->buffer[i * (size + 2) + (size + 1)] = ps_illegal;
    }

    for (uint8_t i = 0; i < size + 2; ++i)
    {
        board->grid[i] = board->buffer + 1 + (i) * (size + 2);
    }
    board->grid = &board->grid[1];

    board->size = size;
    board->turn = c_black;
    board->black_captured = 0;
    board->white_captured = 0;

    return board;
}

void board_destroy (board_t* board)
{
    free (&board->grid[-1]);
    free (board->buffer);
    free (board);
}

pos_state_t board_position_state (board_t* board, uint8_t x, uint8_t y)
{
    return board->grid[x][y];
}

bool board_legal_placement (const board_t* board, uint8_t x, uint8_t y,
                            color_t color)
{
    if (x >= board->size || y >= board->size)
        return false;
    if (board->turn != color)
        return false;
    if (board->grid[x][y] != ps_empty)
        return false;
    if (board_num_liberties (board, get_group(board, x, y), (pos_state_t) color) == 0)
        return false;
    return true;
}

void board_place (board_t* board, uint8_t x, uint8_t y)
{
    board->grid[x][y] = (pos_state_t) board->turn;
    capture(board, x, y);
    board->turn = (board->turn == c_black) ? c_white : c_black;
}

void board_pass (board_t* board)
{
    board->turn = (board->turn == c_black) ? c_white : c_black;
}

uint16_t board_num_liberties (board_t* board, int** group, pos_state_t state)
{		
	uint16_t liberties = 0;		
	
	for(int i = 1; i <= group[0][0]; i++)
	{
		uint8_t a = group[i][0];
		uint8_t b = group[i][1];
		uint8_t left = a-1;
		uint8_t right = a+1;
		uint8_t top = b-1;
		uint8_t bottom = b+1;
							
		if(board_position_state(board, left, b) != ps_white && board_position_state(board, left, b) != ps_black)
			liberties += 1;
		if(board_position_state(board, right, b) != ps_white && board_position_state(board, right, b) != ps_black)
			liberties += 1;
		if(board_position_state(board, a, top) != ps_white && board_position_state(board, a, top) != ps_black)
			liberties += 1;
		if(board_position_state(board, a, bottom) != ps_white && board_position_state(board, a, bottom) != ps_black)
			liberties += 1;
	}			
}

int** get_group(board_t* board, uint8_t x, uint8_t y)
{
	pos_state_t state = board_position_state(board, x, y);
	int size = 1;
	int** group; // FIXME: uninitialised pointer
	group[1][0]=x;
	group[1][1]=y;
	int current = 1;
	int index = 2;

	while(group[current][0] != 0)
	{
		uint8_t a = group[current][0];
		uint8_t b = group[current][1];
		uint8_t left = a-1;
		uint8_t right = a+1;
		uint8_t top = b-1;
		uint8_t bottom = b+1;
		
		while(board_position_state(board, left, b)==state)
		{
				if(board_position_state(board, left, b)!= ps_marked)
				{
					group[index][0]=left;
					group[index][1]=b;	
					board->grid[left][b]=ps_marked;
					++index;
					++size;
				}
			left-=1;
		}
		while(board_position_state(board, right, b)==state)
		{
				if(board_position_state(board, right, b)!= ps_marked)
				{
					group[index][0]=right;
					group[index][1]=b;	
					board->grid[right][b]=ps_marked;
					++index;
					++size;
				}
		
			right+=1;
		}
		while(board_position_state(board, a, top)==state)
		{
				if(board_position_state(board, a, top)!= ps_marked)
				{
					group[index][0]=a;
					group[index][1]=top;	
					board->grid[a][top]=ps_marked;
					++index;
					++size;
				}
			top-=1;			

		}
		while(board_position_state(board, a, bottom)==state)
		{
				if(board_position_state(board, a, bottom)!= ps_marked)
				{
					group[index][0]=a;
					group[index][1]=bottom;	
					board->grid[a][bottom]=ps_marked;
					++index;
					++size;
				}
				
			bottom+=1;
		}
		
		++current;				 
	}		
	group[0][0]=size;
	
	for(int j=1; j<=size; j++)
	{
		board->grid[group[j][0]][group[j][1]]=state;
	}

    // FIXME: return value
}

void capture(board_t* board, uint8_t x, uint8_t y)
{
	pos_state_t state= board_position_state(board, x, y);
	int ***neighbours; // FIXME: uninitialised pointer
	int stones_captured = 0;
	uint8_t left = x-1;
	uint8_t right = x+1;
	uint8_t top = y-1;
	uint8_t bottom = y+1;

	neighbours[0]=get_group(board, left, y);
	neighbours[1]=get_group(board, right, y);
	neighbours[2]=get_group(board, x, top);
	neighbours[3]=get_group(board, x, bottom);
	
	for(int i = 0; i<4; i++)
	{
		if(board_num_liberties(board, neighbours[i], state)==0)
		{
			
			for(int j = 1; j <= neighbours[i][0][0]; j++)
			{
				board->grid[neighbours[i][j][0]][neighbours[i][j][1]] = ps_empty;
				++stones_captured;
			}
		}
	}
	
	if(state = ps_white)
		board->white_captured+=stones_captured;
	else
		board->black_captured+=stones_captured;
}

int score(board_t* board, uint8_t size, uint8_t komi)
{
	int final_score;
	int *groups_white;
	int *groups_black;
	int size_white;
	int size_black;
	uint8_t ind_white = 1;
	uint8_t ind_black = 1;

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			if(board_position_state(board, i, j)==ps_empty)
			{
				int **group = get_group(board, i, j);	
				pos_state_t state;

				if(i > 0)
				{
					int a = i-1;
					state = board_position_state(board, a, j);
				}
				else
				{
					int a = i+1;
					state = board_position_state(board, a, j);
				}
				
				if(state = ps_white)
				{
					groups_white[ind_white]=group[0][0];
					++size_white;
				}
				else
				{
					groups_black[ind_black]=group[0][0];
					++size_white;
				}
			}
		}
	}

	final_score = (int) score_black(board, groups_black) - (int) score_white(board, groups_white) - komi;
}

uint8_t score_white(board_t* board, int *groups_white)
{
	uint8_t score=board->white_captured;

	for(int i = 0; i <= groups_white[0]; i++)
	{
		score+=(int) groups_white[i];
	}
		
	return score;
}

uint8_t score_black(board_t* board, int *groups_black)
{
	uint8_t score=board->black_captured;
	
	for(int i = 0; i <= groups_black[0]; i++)
	{
		score+=(int) groups_black[i];
	}
	
	return score;
}

int approximate_move(board_t* board, uint8_t x, uint8_t y, uint8_t size)
{
	int **spots;
	uint8_t index = 0;
	color_t color = board->turn;
		
	for(int i = 0; i<size; i++)
	{
		for(int j = 0; j<size; j++)
		{
			if(board_position_state(board, i, j)==ps_empty && board_legal_placement(board, i, j, color))
			{
				spots[index][0]=i;
				spots[index][1]=j;	
			}
		}
	}
	
	if(index == 0)
		return -1;
	else
	{
		uint8_t closest = 0;
		for(int a = 0; a <=index; a++)
		{
			if((spots[a][0] < spots[closest][0] && (x - spots[a][0] + y - spots[a][1]) <= (x - spots[closest][0] + y - spots[closest][1])) ||
			  (spots[a][1] < spots[closest][1] && (x - spots[a][0] + y - spots[a][1]) <= (x - spots[closest][0] + y - spots[closest][1])))
			{
				closest = a;
			} 
		}	
	
		board_place(board, spots[closest][1], spots[closest][2]);
		return 1;
	}		
}
