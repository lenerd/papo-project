#include "ngg_game.h"
#include "neuralnet/neuralnet.h"
#include "go/game.h"
#include "go/player.h"
#include "util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void init_opts (options_t* opts)
{
    memset (opts, 0x00, sizeof (options_t));
}


void cleanup_opts (options_t* opts)
{
}

int unsupervised_check_options (options_t* opts)
{
    int ret = EXIT_SUCCESS;
    if (!opts->set_i)
    {
        fprintf (stderr, "error: specify input file with -i\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_n)
    {
        fprintf (stderr, "error: specify number of generations with -n\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_o)
    {
        fprintf (stderr, "error: specify output file with -o\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_s)
    {
        fprintf (stderr, "error: specify size of go board with -s\n");
        ret = EXIT_FAILURE;
    }
    return ret;
}

int check_board_size (nnet_set_t* set, size_t board_size)
{
    for (size_t i = 0; i < set->size; ++i)
    {
        if (set->nets[i]->neurons_per_layer[0] != board_size * board_size ||
            set->nets[i]->neurons_per_layer[set->nets[i]->layer_count - 1] !=
                board_size * board_size)
        {
            fprintf (stderr, "net #%zu has an illegal number of i/o neurons\n",
                     i);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}


int unsupervised (options_t* opts)
{
    int ret = unsupervised_check_options (opts);
    if (ret)
        return ret;

    nnet_set_t* set = nnet_set_from_file (opts->in_path, opts->b_in);

    ret = check_board_size (set, opts->board_size);
    if (ret)
    {
        nnet_set_destroy (set);
        return ret;
    }

    int64_t* scores = SAFE_MALLOC (set->size * sizeof (int64_t));

    for (size_t i = 0; i < opts->n; ++i)
    {
        if (i)
        {
            // TODO:
            // next generation
        }
        memset (scores, 0x00, set->size * sizeof (int64_t));

        for (size_t net_1 = 0; net_1 < set->size; ++net_1)
            for (size_t net_2 = 0; net_2 < set->size; ++net_2)
            {
                if (net_1 == net_2)
                    continue;
                player_t* p1 = player_create_net (set->nets[net_1]);
                player_t* p2 = player_create_net (set->nets[net_2]);
                game_t* game = game_create (p1, p2, opts->board_size, 1024);

                while (!game->finished)
                    game_step (game);

                // save scores
                int64_t score = game_score (game);
                game_destroy (game);
                scores[net_1] += score;
                scores[net_2] -= score;
            }
    }

    nnet_set_to_file (set, opts->out_path, opts->b_out);
    nnet_set_destroy (set);

    return EXIT_SUCCESS;
}
