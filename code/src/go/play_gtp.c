#include <stdio.h>
#include "play_gtp.h"
#include "gtp.h"
#include "neuralnet/neuralnet.h"
#include "board.h"
#include "game_controller"

//State of the game
float komi;
board_t* board;


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
}

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

	for(inti = 0; commands[i].name != NULL; ++i)
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

	if(board_size < MIN_SIZE || MAX_SIZE < board_size)
		return gtp_failure(id, "size is not acceptable");	

	gtp_internal_set_boardsize(board_size);
	board = board_create(board_size);
	return gtp_success(id, " ");
}

static int gtp_clear_board()
{
	remove(//File)
	fopen(//New file)
	board_t* board = board_create(gtp_boardsize);
	return gtp_success(id, " ");
}

static int gtp_komi(char* s, int id)
{	
	float new_komi;
	if(sscanf(s, "%f", &new_komi < 0)
		return gtp_failure(id, "not an integer");

	komi = new_komi;
	return GTP_QUIT;
}

static int gtp_play(char* s, int id)
{
	board_place_color();
	return GTP_QUIT;
}

static int gtp_genmove(char* s, int id)
{
	

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

	return gtp_success(id, "d", board_legal_placement(board, x, y, color);	
}
