#include "ngg_game.h"
#include "neuralnet/neuralnet.h"
#include "go/game.h"
#include "go/player.h"
#include "genetic/genetic_algorithm.h"
#include "util/util.h"
#include "util/mpi.h"
#include "util/scheduler.h"

#include <assert.h>
#include <inttypes.h>
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int master (options_t* opts, process_info_t* pinfo, nnet_set_t* set)
{
    size_t games = set->size * set->size;
    size_t start =
        (size_t)((double) (set->size * set->size) * opts->sched_initial);
    size_t chunk_size = opts->sched_chunksize;
    int sent_done = 0;
    MPI_Status status;
    partition_t part;

    while (start < games)
    {
        part.start_x = start / set->size;
        part.start_y = start % set->size;
        part.len = games - start > chunk_size ? chunk_size : games - start;
        MPI_Recv (NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
                  &status);
        MPI_Send (&part, 3, MPI_UINT64_T, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

        start += chunk_size;
    }
    part.len = 0;
    while (sent_done < pinfo->mpi_size - 1)
    {
        MPI_Recv (NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
                  &status);
        MPI_Send (&part, 3, MPI_UINT64_T, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        ++sent_done;
    }

    return 0;
}

int worker (options_t* opts, process_info_t* pinfo, nnet_set_t* set,
            size_t* wins, generation_stats_t* stats)
{
    partition_t part;
    create_initial_partition (&part, pinfo, set->size, opts->sched_initial);

    do
    {
        size_t net_1 = part.start_x;
        size_t net_2 = part.start_y;
        size_t count = 0;

        for (; count < part.len; ++net_1)
        {
            player_t* p1 = player_create_net (set->nets[net_1], ver1);
            for (; net_2 < set->size && count < part.len; ++net_2)
            {
                ++count;

                if (net_1 == net_2)
                    continue;

                player_t* p2 = player_create_net (set->nets[net_2], ver1);
                game_t* game = game_create (p1, p2, opts->board_size, 1024);

                /* play */
                while (!game->finished)
                    game_step (game);

                /* update fitness */
                int64_t score = game_score (game);
                if (score > 0)
                    ++wins[net_1];
                else if (score < 0)
                    ++wins[net_2];

                /* update generation stats */
                stats->play_cnt += game->play_cnt;
                stats->pass_cnt += game->pass_cnt;

                game_destroy (game);
                player_destroy (p2);
            }
            net_2 = 0;

            player_destroy (p1);
        }

        MPI_Send (NULL, 0, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv (&part, 3, MPI_UINT64_T, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
                  MPI_STATUS_IGNORE);
    } while (part.len != 0);

    return 0;
}


int unsupervised (options_t* opts, int argc, char** argv)
{
    assert (opts != NULL);

    int ret, rc;
    process_info_t pinfo;

    nnet_set_t* set = NULL;
    genome_t** genomes = NULL;
    population_t* pop = NULL;

    /* check required options */
    ret = unsupervised_check_options (opts);
    if (ret)
        return ret;

    /* start MPI */
    rc = MPI_Init (&argc, &argv);
    if (rc != MPI_SUCCESS)
    {
        fprintf (stderr, "[%s:%u] MPI_Init failed (%d)\n", __FILE__, __LINE__,
                 rc);
        return rc;
    }
    MPI_Comm_rank (MPI_COMM_WORLD, &pinfo.mpi_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &pinfo.mpi_size);
    if (pinfo.mpi_size == 1)
    {
        fprintf (stderr, "the master has no worker to direct (at least 2 "
                         "processes required)\n");
        return EXIT_FAILURE;
    }

    /* seed random number generator */
    if (opts->set_seed)
        srand (opts->seed);
    else
    {
        unsigned seed = 0;
        if (pinfo.mpi_rank == 0)
        {
            srand ((unsigned) time (0));
            seed = (unsigned) random ();
        }
        MPI_Bcast (&seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        srand (seed);
    }

    /* load neural networks */
    set = nnet_set_from_file (opts->in_path, opts->b_in);
    ret = check_board_size (set, opts->board_size);
    if (ret)
    {
        nnet_set_destroy (set);
        return ret;
    }

    /* prepare population for the genetic algorithm */
    genomes = SAFE_MALLOC (set->size * sizeof (genome_t*));
    for (size_t i = 0; i < set->size; ++i)
        genomes[i] =
            genome_create (set->nets[i]->edge_count, &set->nets[i]->edge_buf,
                           &update_neuralnet, set->nets[i]);
    pop = population_create (set->size, genomes, 1.0f, 0.01f);

    /* fitness values */
    uint64_t* wins = SAFE_MALLOC (set->size * sizeof (uint64_t));

    /* statistic */
    generation_stats_t stats;
    generation_stats_t total_stats = {0, 0, 0, 0, {0, 0}};
    uint64_t game_cnt = set->size * (set->size - 1);
    struct timespec start, end;
    struct timespec diff;

    /* csv header */
    if (pinfo.mpi_rank == 0 && !opts->human_readable)
        csv_header (stdout);

    /* main loop */
    for (size_t gen = 0; gen < opts->n; ++gen)
    {
        /* reset stats */
        memset (&stats, 0x00, sizeof (generation_stats_t));
        stats.generation = gen;
        stats.game_cnt = game_cnt;

        /* start generation time */
        clock_gettime (CLOCK_MONOTONIC, &start);
        memset (wins, 0x00, set->size * sizeof (uint64_t));

        /* do the work */
        if (pinfo.mpi_rank == 0)
            master (opts, &pinfo, set);
        else
            worker (opts, &pinfo, set, wins, &stats);

        /* share the wins */
        MPI_Allreduce (MPI_IN_PLACE, wins, (int) set->size, MPI_UINT64_T,
                       MPI_SUM, MPI_COMM_WORLD);
        for (size_t net = 0; net < set->size; ++net)
            pop->individuals[net]->fitness = (float) wins[net];

        // Make it so!
        the_next_generation (pop);

        // Make it so!
        the_next_generation (pop);

        /* end generation time */
        clock_gettime (CLOCK_MONOTONIC, &end);
        diff = diff_timespec (start, end);
        stats.generation_time = sum_timespec (stats.generation_time, diff);

        /* save additional statistics */
        if (pinfo.mpi_rank == 0)
        {
            MPI_Reduce (MPI_IN_PLACE, &stats.play_cnt, 1, MPI_UINT64_T, MPI_SUM,
                        0, MPI_COMM_WORLD);
            MPI_Reduce (MPI_IN_PLACE, &stats.pass_cnt, 1, MPI_UINT64_T, MPI_SUM,
                        0, MPI_COMM_WORLD);
            stats_add (&total_stats, &stats);
        }
        else
        {
            MPI_Reduce (&stats.play_cnt, NULL, 1, MPI_UINT64_T, MPI_SUM, 0,
                        MPI_COMM_WORLD);
            MPI_Reduce (&stats.pass_cnt, NULL, 1, MPI_UINT64_T, MPI_SUM, 0,
                        MPI_COMM_WORLD);
        }

        /* print informational output */
        if (pinfo.mpi_rank == 0)
        {
            if (opts->human_readable)
            {
                printf ("#\twins\tscores:\n");
                for (size_t net = 0; net < set->size; ++net)
                    printf ("%2zu\t%" PRId64 "\t%f\n", net, wins[net],
                            (double) genomes[net]->fitness);

                human_output (&stats, stdout);
            }
            else
            {
                csv_line (&stats, stdout);
            }
        }
    }

    /* print accumulated stats */
    if (pinfo.mpi_rank == 0 && !opts->human_readable)
    {
        fprintf (stdout, "# total: ");
        csv_line (&total_stats, stdout);
    }

    /* save trained networks */
    if (pinfo.mpi_rank == 0)
    {
        nnet_set_to_file (set, opts->out_path, opts->b_out);
    }

    /* cleanup */
    free (wins);
    population_destroy (pop);
    free (genomes);
    nnet_set_destroy (set);

    /* end MPI */
    rc = MPI_Finalize ();
    if (rc != MPI_SUCCESS)
    {
        fprintf (stderr, "[%s:%u] MPI_Finalize failed (%d)\n", __FILE__,
                 __LINE__, rc);
        return rc;
    }

    return EXIT_SUCCESS; /* :) */
}
