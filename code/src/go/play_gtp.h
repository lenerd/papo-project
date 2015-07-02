#include <stdio.h>
#include <stdio.h>
#include "play_gtp.h"
#include "gtp.h"
#include "neuralnet/neuralnet.h"
#include "board.h"

/*
* /brief Prints out GTP version
*/
static int protocol_version();

/*
* /brief Prints out name of the engine
*/
static int name();

/*
* /brief Prints out the engine version
*/
static int version();

/*
* /brief Confirms if command is known or not
*/
static int known_command(char* command_name);

/*
* /brief Lists all known commands
*/
static int list_commands();

/*
* /brief Ends session
*/
static int quit();

/*
* /brief Changes board size, clears board
*/
static int boardsize(int size);

/*
* /brief Clears board
*/
static int clear_board();

/*
* /brief Komi is changed
*/
static int komi(float new_komi);

/*
* /brief Executes move
*/
static int play(move);

/*
* /brief Generates move for color given
*/
static int genmove(color);
