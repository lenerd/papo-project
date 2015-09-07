#include "ngg_test.h"
#include "neuralnet/neuralnet.h"
#include "go/game.h"
#include "go/player.h"
#include "util/util.h"
#include "util/misc.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void init_opts (options_t* opts)
{
    memset (opts, 0x00, sizeof (options_t));
}

int test_check_options (options_t* opts)
{
    int ret = EXIT_SUCCESS;
    if (!opts->set_i)
    {
        fprintf (stderr, "error: specify first input file with -i\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_s)
    {
        fprintf (stderr, "error: specify size of go board with -s\n");
        ret = EXIT_FAILURE;
    }
    return ret;
}


int test (options_t* opts)
{
    int ret = test_check_options (opts);
    if (ret)
        return ret;


    srand ((unsigned) time(0));


    /* load neural networks */
    nnet_set_t* set1 = nnet_set_from_file (opts->in_path1, opts->b_in);
    size_t neurons[] = {81, 82, 82, 82, 82};
    nnet_set_t* set2 = nnet_set_create (32);
    for (size_t i = 0; i < 32; ++i)
        set2->nets[i] = nnet_create_random(5, neurons);
    ret = check_board_size (set1, opts->board_size, opts->ver1);
    ret = ret || check_board_size (set2, opts->board_size, opts->ver1);
    if (ret)
    {
        nnet_set_destroy (set1);
        nnet_set_destroy (set2);
        return ret;
    }

    /* fitness values */
    uint64_t wins[2] = {0, 0};
    int64_t score;
    game_t* game;
    player_t* p1;
    player_t* p2;

    for (size_t net_1 = 0; net_1 < set1->size; ++net_1)
    {
        p1 = player_create_net (set1->nets[net_1], ver0);

        for (size_t net_2 = 0; net_2 < set2->size; ++net_2)
        {
            p2 = player_create_net (set2->nets[net_2], ver0);

            game = game_create (p1, p2, opts->board_size, 1024);

            /* play */
            while (!game->finished)
                game_step (game);

            score = game_score (game);
            if (score > 0)
                ++wins[0];
            else if (score < 0)
                ++wins[1];

            game_destroy (game);

            game = game_create (p2, p1, opts->board_size, 1024);

            /* play */
            while (!game->finished)
                game_step (game);

            score = game_score (game);
            if (score > 0)
                ++wins[1];
            else if (score < 0)
                ++wins[0];

            game_destroy (game);

            player_destroy (p2);
        }

        player_destroy (p1);
    }

    printf ("input: %" PRIu64 ", random: %" PRIu64 "\n", wins[0], wins[1]);


    nnet_set_destroy (set1);
    nnet_set_destroy (set2);

    return EXIT_SUCCESS;
}
