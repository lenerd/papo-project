#include "ngg_game.h"
#include "neuralnet/neuralnet.h"
#include "go/game.h"
#include "go/player.h"
#include "genetic/genetic_algorithm.h"
#include "util/util.h"
#include "util/mpi.h"

#include <mpi.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void init_opts (options_t* opts)
{
    memset (opts, 0x00, sizeof (options_t));
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
                board_size * board_size + 1)
        {
            fprintf (stderr, "net #%zu has an illegal number of i/o neurons\n",
                     i);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}


int unsupervised (options_t* opts, int argc, char** argv)
{
    int ret, rc;
    process_info_t pinfo;
    partition_t part;
    nnet_set_t* set = NULL;
    genome_t** genomes = NULL;
    population_t* pop = NULL;

    ret = unsupervised_check_options (opts);
    if (ret)
        return ret;


    rc = MPI_Init (&argc, &argv);
    if (rc != MPI_SUCCESS)
    {
        fprintf (stderr, "[%s:%u] MPI_Init failed (%d)\n", __FILE__, __LINE__,
                 rc);
        return rc;
    }
    MPI_Comm_rank (MPI_COMM_WORLD, &pinfo.mpi_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &pinfo.mpi_size);

    /* seed random number generator */
    srand ((unsigned int) time (0));

    /* load neural networks */
    set = nnet_set_from_file (opts->in_path, opts->b_in);
    ret = check_board_size (set, opts->board_size);
    if (ret)
    {
        nnet_set_destroy (set);
        return ret;
    }

    /* prepare population for the genetic algorithm */
    if (pinfo.mpi_rank == 0)
    {
        genomes = SAFE_MALLOC (set->size * sizeof (genome_t*));
        for (size_t i = 0; i < set->size; ++i)
            genomes[i] =
                genome_create (set->nets[i]->edge_count, &set->nets[i]->edge_buf,
                               &update_neuralnet, set->nets[i]);
        pop = population_create (set->size, genomes, 1.0f);
    }

    /* fitness values */
    uint64_t* wins = SAFE_MALLOC (set->size * sizeof (uint64_t));

    /* statistic */
    uint64_t move_cnt = 0;
    uint64_t play_cnt = 0;
    uint64_t pass_cnt = 0;
    uint64_t game_cnt = set->size * (set->size - 1);
    uint64_t move_total = 0;
    uint64_t play_total = 0;
    uint64_t pass_total = 0;
    uint64_t game_total = 0;
    struct timespec gen_time = {0, 0};
    struct timespec total_time = {0, 0};
    struct timespec start, end;

    /* csv header */
    if (pinfo.mpi_rank == 0)
    {
        if (!opts->human_readable)
            printf ("# generation,time,plays,passes\n");
    }


    create_partition (&part, &pinfo, set->size);

    for (size_t gen = 0; gen < opts->n; ++gen)
    {
        if (pinfo.mpi_rank == 0)
        {
            if (opts->human_readable)
            {
                printf ("==============\n");
                printf ("generation %zu\n", gen);
            }
        }

        if (gen)
        {
            if (pinfo.mpi_rank == 0)
            {
                the_next_generation (pop);
                broadcast_nnet_set (&set, 0);
            }
            else
            {
                nnet_set_destroy (set);
                broadcast_nnet_set (&set, 0);
            }
        }

        /* reset stats */
        memset (wins, 0x00, set->size * sizeof (uint64_t));
        move_cnt = 0;
        play_cnt = 0;
        pass_cnt = 0;

        for (size_t net_1 = part.start; net_1 < part.end; ++net_1)
        {
            player_t* p1 = player_create_net (set->nets[net_1]);

            for (size_t net_2 = 0; net_2 < set->size; ++net_2)
            {
                if (net_1 == net_2)
                    continue;
                player_t* p2 = player_create_net (set->nets[net_2]);
                game_t* game = game_create (p1, p2, opts->board_size, 1024);

                // timespec_get (&start, TIME_UTC);
                clock_gettime (CLOCK_MONOTONIC, &start);

                /* play */
                while (!game->finished)
                    game_step (game);

                // timespec_get (&end, TIME_UTC);
                clock_gettime (CLOCK_MONOTONIC, &end);
                gen_time = diff_timespec (start, end);
                total_time = sum_timespec (total_time, gen_time);

                /* update fitness */
                int64_t score = game_score (game);
                if (score > 0)
                    ++wins[net_1];
                else if (score < 0)
                    ++wins[net_2];

                /* update generation stats */
                move_cnt += game->move_cnt;
                play_cnt += game->play_cnt;
                pass_cnt += game->pass_cnt;

                game_destroy (game);
                player_destroy (p2);
            }

            player_destroy (p1);
        }

        /* accumulated stats */
        move_total += move_cnt;
        play_total += play_cnt;
        pass_total += pass_cnt;

        if (pinfo.mpi_rank == 0)
        {
            MPI_Reduce (MPI_IN_PLACE, wins, set->size, MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);
            for (size_t net = 0; net < set->size; ++net)
                pop->individuals[net]->fitness = (float) wins[net];
        }
        else
        {
            MPI_Reduce (wins, NULL, set->size, MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);
        }

        game_total += game_cnt;

        if (pinfo.mpi_rank == 0)
        {
            if (opts->human_readable)
            {
                printf ("#\twins\tscores:\n");
                for (size_t net = 0; net < set->size; ++net)
                    printf ("%2zu\t%" PRId64 "\t%f\n", net, wins[net],
                            (double) genomes[net]->fitness);

                /* print stats */
                putchar ('\n');
                printf ("game time: ");
                print_time (total_time);
                printf ("games: %" PRIu64 "\n", game_total);
                printf ("games/s: %f\n",
                        (double) game_total / timespec_to_double (total_time));
                printf ("moves: %" PRIu64 "\n", move_total);
                printf ("moves/s: %f\n",
                        (double) move_total / timespec_to_double (total_time));
                printf ("moves/game: %f\n",
                        (double) move_total / (double) game_total);
                printf ("plays: %" PRIu64 "\n", play_total);
                printf ("passes: %" PRIu64 "\n", pass_total);
                printf ("passes/s: %f\n",
                        (double) pass_total / timespec_to_double (total_time));
                printf ("passes/game: %f\n",
                        (double) pass_total / (double) game_total);
                printf ("passes/moves: %f\n",
                        (double) pass_total / (double) move_total);
                // print_time (div_timespec (game_time, game_total));
            }
            else
            {
                // TODO: CSV output
                printf ("%" PRId64 ",", gen);
                printf ("%f,", timespec_to_double (gen_time));
                printf ("%" PRId64 ",", play_cnt);
                printf ("%" PRId64, pass_cnt);
                putchar ('\n');
            }
        }
    }

    free (wins);
    if (pinfo.mpi_rank == 0)
    {
        population_destroy (pop);
        free (genomes);
        nnet_set_to_file (set, opts->out_path, opts->b_out);
    }
    nnet_set_destroy (set);

    rc = MPI_Finalize ();
    if (rc != MPI_SUCCESS)
    {
        fprintf (stderr, "[%s:%u] MPI_Finalize failed (%d)\n", __FILE__,
                 __LINE__, rc);
        return rc;
    }

    return EXIT_SUCCESS;
}
