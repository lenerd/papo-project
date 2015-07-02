#include <stdio.h>
#include "play_gtp.h"
#include "gtp.h"
#include "neuralnet/neuralnet.h"
#include "board.h"



static int protocol_version(char* s, int id)
{
	gtp_success(id, "1 or 2, not quite sure");
	return GTP_QUIT;
}

static int name(char* s, int id)
{
	gtp_success(id, "Neugengo");
	return GTP_QUIT;
}

static int version(char* s, int id)
{
	gtp_success(id, " ");
	return GTP_QUIT;
}

static int known_commands(char* s, int id)
{
	return GTP_QUIT;
}

static int list_commands(char* s, int id)
{
	return GTP_QUIT;
}

static int quit(char* s, int id)
{
	gtp_success(id, " ");
	return GTP_QUIT;
}

static int boardsize(char* s, int id)
{
	int board_size;
	if(sscanf(s, "%d", &board_size)<1)
		return gtp_failure(id, "not an integer");

	if(board_size < MIN_SIZE || MAX_SIZE < board_size)
		return gtp_failure(id, "size is not acceptable");	

	clear_board();
	gtp_internal_set_boardsize(board_size);
	board = board_create(board_size);
	return gtp_success(id, " ");
}

static int clear_board()
{
	return gtp_success(id, " ");
}

static int komi(char* s, int id)
{
	gtp_success(id, "1 or 2, not quite sure");
	return GTP_QUIT;
}

static int play(char* s, int id)
{
	gtp_success(id, "1 or 2, not quite sure");
	return GTP_QUIT;
}

static int genmove(char* s, int id)
{
	gtp_success(id, "1 or 2, not quite sure");
	return GTP_QUIT;
}
