#include <stdio.h>
#include <stdio.h>
#include "go/play_gtp.h"
#include "go/gtp.h"
#include "neuralnet/neuralnet.h"
#include "go/board.h"

/*
* /brief Prints out GTP version
*/
static int gtp_protocol_version(char* s, int id);

/*
* /brief Prints out name of the engine
*/
static int gtp_name(char* s, int id);

/*
* /brief Prints out the engine version
*/
static int gtp_version(char* s, int id);

/*
* /brief Confirms if command is known or not
*/
static int gtp_known_command(char* s, int id);

/*
* /brief Lists all known commands
*/
static int gtp_list_commands(char* s, int id);

/*
* /brief Ends session
*/
static int gtp_quit(char* s, int id);

/*
* /brief Changes board size, clears board
*/
static int gtp_boardsize(char* s, int id);

/*
* /brief Clears board
*/
static int gtp_clear_board(char* s, int id);

/*
* /brief Komi is changed
*/
static int gtp_komi(char* s, int id);

/*
* /brief Executes move
*/
static int gtp_playmove(char* s, int id);

/*
* /brief Generates move for color given
*/
static int gtp_genmove(char* s, int id);

/*
 * /brief Tests if move is legal
 */
static int gtp_is_legal(char* s, int id);
