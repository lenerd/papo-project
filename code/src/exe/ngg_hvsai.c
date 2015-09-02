#define _GNU_SOURCE 1

#include "ngg_hvsai.h"

#include "neuralnet/neuralnet.h"
#include "util/util.h"

#include "go/board.h"
#include "go/player.h"
#include "go/game.h"
#include "go/record.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <argp.h>

//#include <inttypes.h>
//#include <unistd.h>

static int parse_opt (int key, char* arg, struct argp_state* state){

	options_t* opts = state->input;
    switch (key)
    {
    	case 'i':  // input
	        if (strlen (arg) > 0)
	        {
	            opts->in_path = arg;
	            opts->set_i = true;
	        }
	        else
	        {
	            argp_error (state, "input filename is empty");
	        }
	        break;

    	case 'b': // binary
    		opts->binary = true;
    		break;
    	
    	case 's':  // board-size
	    {
	        size_t tmp = strtoul (arg, NULL, 10);
	        if (tmp > 1)
	        {
	            opts->board_size = tmp;
	            opts->set_s = true;
	        }
	        else
	        {
	            argp_error (state, "a minimum board size of 2x2 is required");
	        } 
	        break;
	    }
	}
	
	return 0;

}

static void play(neuralnet_t* net){

	player_t* p1 = player_create_human ();
    // player_t* p2 = player_create_human ();
    //size_t neurons[] = {81, 81, 81, 81, 81};
    size_t neurons[] = {25, 25, 25, 25, 26};
    player_t* p2 = player_create_net (net);

    game_t* game = game_create (p1, p2, 5, 10);
    FILE* file = fopen ("record", "w");
    FILE* file2 = fopen ("game.sgf", "w");
    game_add_recorder (game, recorder_ascii_create(game->board, file));
    game_add_recorder (game, recorder_sgf_create(game->board, file2));

    while (!game->finished)
    {
        game_step(game);
        printf("\n");
        board_print (game->board, stdout);
        printf("\nScore: %" PRId64 "\n", game_score (game));
        printf("Moves: %" PRId64 "\n", game->move_cnt);
        usleep(200000);
    }

    printf("\nScore: %" PRId64 "\n", game_score (game));
    printf("Moves: %" PRId64 "\n", game->move_cnt);

    game_destroy (game);
    nnet_destroy (net);
    fclose (file);
    fclose (file2);

}

int main(int argc, char** argv){

	hvsai_opts_t opts;

    struct argp_option options[] = {
        {"in", 'i', "FILE", 0, "load neuralnets from file", 0},
        {"binary", 'b', 0, 0, "use binary files", 0},
        {"board-size", 's', "NUM", 0, "size of the used go board", 0},
        {0, 0, 0, 0, 0, 0}};
    struct argp argp = {options, &parse_opt, 0, 0, 0, 0, 0};
    argp_parse (&argp, argc, argv, 0, 0, &opts);

    nnet_set_t* nets = nnet_set_from_file (opts->in_path, opts->b_in);

    int ret = EXIT_SUCCESS;
    switch (opts.action)
    {
    case calc:
        ret = calculate (&opts);
        break;
    case create:
        ret = create_networks (&opts);
        break;
    case gen_data:
        ret = generate_training_data (&opts);
        break;
    case train:
        ret = train_networks (&opts);
        break;
    default:
        break;
    }
    return ret;

	return EXIT_SUCCESS;

}