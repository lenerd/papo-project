#include <stdio.h>
#include "play_gtp.h"
#include "neuralnet/neuralnet.h"
#include "board.h"
#include "game.h"
#include "player.h"

//State of the game/engine
static float komi_value;
static game_t* game;

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

static int start_game()
{
	player_t* p1 = 
	player_t* p2 =
	game = game_create(p1, p2, gtp_boardsize, 0)
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
	for(int i = 0; commands[i].name != NULL; ++i)
	{
		if(strcmp(s, commands[i].name) == 0)	
			gtp_printf("Yup, I know that.\n");
	}

	gtp_printf("Sorry, command is unknown. \n");
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
	game->board = board_create(board_size);
	return gtp_success(id, " ");
}

static int gtp_clear_board(char* s, int id)
{
	game->board = board_create(gtp_boardsize);
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
		board_pass(game->board);
		return gtp_success(id, " ");
	}	

	if(!gtp_decode_move(s, &c, &x, &y))
		return gtp_failure(id, "not a valid move");
	
	if(c == 1)
		color = c_white;
	else
		color = c_black;

	if(!board_legal_placement(game->board, x, y, color))
		return gtp_failure(id, "move is illegal");

	board_place_color(game->board, x, y, board->turn);
	return gtp_success(id, " ");
}

static int gtp_genmove(char* s, int id)
{
	if(game->board->turn == c_black)
		player_t* player = game->black;
	else
		player_t* player = game->white;
	
	position_t pos;
	pos = player_move (p, game->board);
        if (board_legal_placement (game->board, pos.x, pos.y, game->turn))
        {
                board_place (game->board, pos.x, pos.y);
                game->passed = false;
                ++game->play_cnt;
		gtp_printid(id, GTP_SUCCESS);
  		gtp_print_vertex(pos.x, pos.y);
        }
        else
        {
            board_pass (game->board);
            ++game->pass_cnt;
	    gtp_printf("PASS \n");
            if (game->passed)
                game->finished = true;
            game->passed = true;
        }

	return gtp_finish_response();
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

	return gtp_success(id, "d", board_legal_placement(game->board, x, y, color));	
}
