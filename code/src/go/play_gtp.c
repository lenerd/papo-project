#include <stdio.h>
#include "play_gtp.h"
#include "newneunet/newneunet.h"
#include "board.h"
#include "game_controller.c"

//State of the game/engine
float komi_value;
board_t* board;
//TODO: Fix initialisation, needs a trained net
neuralnet_t* net = create_neural_net_random(5, 4, 4, 2);

#define DECLARE(func) static int func(char* s, int id)

DECLARE(gtp_protocol_version); 
DECLARE(gtp_name);
DECLARE(gtp_version);
DECLARE(gtp_known_commands);
DECLARE(gtp_list_commands);
DECLARE(gtp_quit);
DECLARE(gtp_board_size);
DECLARE(gtp_clear_board);
DECLARE(gtp_komi);
DECLARE(gtp_play);
DECLARE(gtp_genmove);
DECLARE(gtp_is_legal);

static struct gtp_command commands[]=
{
	{"protocol version", gtp_protocol_version},
	{"engine name", gtp_name},
	{"engine version", gtp_version},
	{"test for known command", gtp_known_commands},
	{"list all known commands", gtp_list_commands},
	{"quit", gtp_quit},
	{"change board size", gtp_board_size},
	{"clear board", gtp_clear_board},
	{"set komi", gtp_komi},
	{"play one move", gtp_play},
	{"generate a move", gtp_genmove},
	{"check if move is legal", gtp_is_legal},
	{NULL, NULL}
};

static int gtp_protocol_version(char* s, int id)
{
	gtp_success(id, "1 or 2, not quite sure");
	return GTP_OK;
}

static int gtp_name(char* s, int id)
{
	gtp_success(id, "Neugengo");
	return GTP_OK;
}

static int gtp_version(char* s, int id)
{
	gtp_success(id, "0.1");
	return GTP_OK;
}

static int gtp_known_commands(char* s, int id)
{	
	return gtp_succcess();
}

static int gtp_list_commands(char* s, int id)
{
	gtp_printid(id, GTP_SUCCESS);

	for(int i = 0; commands[i].name != NULL; ++i)
	{
		gtp_printf("%s\n", commands[i].name);
	}

	return GTP_OK;
}

static int gtp_quit(char* s, int id)
{
	gtp_success(id, " ");
	return GTP_QUIT;
}

static int gtp_boardsize(char* s, int id)
{
	int board_size;
	if(sscanf(s, "%d", &board_size)<0)
		return gtp_failure(id, "please enter a positive number");

	if(board_size < 2 || 25 < board_size)
		return gtp_failure(id, "size is not acceptable");	

	gtp_internal_set_boardsize(board_size);
	board = board_create(board_size);
	return gtp_success(id, " ");
}

static int gtp_clear_board(char* s, int id)
{
	board_t* board = board_create(gtp_boardsize);
	return GTP_OK; 
}

static int gtp_komi(char* s, int id)
{	
	float new_komi;
	if(sscanf(s, "%f", &new_komi) < 0)
		return gtp_failure(id, "not an integer");

	komi_value = new_komi;
	return gtp_success(id, " ");
}

static int gtp_play(char* s, int id)
{
	int x, y, c;
	color_t color;

	if(strncmp(s, "pass") == 0)
	{
		board_pass(board);
		return gtp_success(id, " ");
	}	

	if(!gtp_decode_move(s, &c, &x, &y))
		return gtp_failure(id, "not a valid move");
	
	if(c == 1)
		color = c_white;
	else
		color = c_black;

	if(!board_legal_placement(board, x, y, color))
		return gtp_failure(id, "move is illegal");

	board_place_color(board, x, y, board->turn);
	return gtp_success(id, " ");
}

static int gtp_genmove(char* s, int id)
{
	int* output = calculate_output(net, board->grid, UINT8);
	
	int c = 0;
	while(c < board->size * board->size)
	{	
		int max = -500;
		int max_p = -1;

		for(int i = 0; i < board->size * board->size; ++i)
		{
			if(output[i] > max)
				max_p = i;			
		}

		if(!board_legal_placement(board, max_p / board->size, max_p % board->size, board->turn))
			++c;
		else
		{
			if((max_p / board->size) == 10)
				board_pass(board);
			else
				board_place_color(board, max_p / board->size, max_p % board->size, board->turn);

			return gtp_success(id, " ");
		}	
	}

	return gtp_failure(id, "no valid move left");		
}

static int gtp_is_legal(char* s, int id)
{
	int x, y, c;
	color_t color;

	if(!gtp_decode_move(s, &c, &x, &y))
		return gtp_failure(id, "not a valid move");

	if(c == 1)
		color = c_white;
	else
		color = c_black;

	return gtp_success(id, "d", board_legal_placement(board, x, y, color));	
}
