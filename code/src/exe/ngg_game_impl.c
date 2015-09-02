#include "ngg_game.h"
#include "neuralnet/neuralnet.h"
#include "go/game.h"
#include "go/player.h"
#include "genetic/genetic_algorithm.h"
#include "util/util.h"
#include "util/mpi.h"
#include "util/scheduler.h"

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


typedef struct
{
    uint64_t generation;
    uint64_t game_cnt;
    uint64_t play_cnt;
    uint64_t pass_cnt;

    struct timespec generation_time;
} generation_stats_t;


void stats_add (generation_stats_t* dst, generation_stats_t* src)
{
    dst->generation =
        dst->generation > src->generation ? dst->generation : src->generation;
    dst->game_cnt += src->game_cnt;
    dst->play_cnt += src->play_cnt;
    dst->pass_cnt += src->pass_cnt;

    dst->generation_time =
        sum_timespec (dst->generation_time, src->generation_time);
}


void human_output (generation_stats_t* stats, FILE* file)
{
    uint64_t move_cnt = stats->play_cnt + stats->pass_cnt;

    /* print stats */
    fprintf (file, "==============\n");
    fprintf (file, "generation %zu\n\n", stats->generation);
    fprintf (file, "generation time: ");
    print_time (stats->generation_time, file);
    fputc ('\n', file);
    fputc ('\n', file);
    fprintf (file, "games: %" PRIu64 "\n", stats->game_cnt);
    fprintf (file, "games/s: %f\n",
             (double) stats->game_cnt / timespec_to_double (stats->generation_time));
    fprintf (file, "moves: %" PRIu64 "\n", move_cnt);
    fprintf (file, "moves/s: %f\n",
             (double) move_cnt / timespec_to_double (stats->generation_time));
    fprintf (file, "moves/game: %f\n",
             (double) move_cnt / (double) stats->game_cnt);
    fprintf (file, "plays: %" PRIu64 "\n", stats->play_cnt);
    fprintf (file, "passes: %" PRIu64 "\n", stats->pass_cnt);
    fprintf (file, "passes/s: %f\n",
             (double) stats->pass_cnt / timespec_to_double (stats->generation_time));
    fprintf (file, "passes/game: %f\n",
             (double) stats->pass_cnt / (double) stats->game_cnt);
    fprintf (file, "passes/moves: %f\n",
             (double) stats->pass_cnt / (double) move_cnt);
}

void csv_header (FILE* file)
{
    fprintf (file, "# generation,generation time,game count,play "
                   "count,pass count\n");
}

void csv_line (generation_stats_t* stats, FILE* file)
{
    char delimiter = ',';
    fprintf (file, "%" PRId64, stats->generation);
    fputc (delimiter, file);
    fprintf (file, "%f", timespec_to_double (stats->generation_time));
    fputc (delimiter, file);
    fprintf (file, "%" PRId64, stats->game_cnt);
    fputc (delimiter, file);
    fprintf (file, "%" PRId64, stats->play_cnt);
    fputc (delimiter, file);
    fprintf (file, "%" PRId64, stats->pass_cnt);
    putchar ('\n');
}

int master (process_info_t* pinfo, nnet_set_t* set)
{
    size_t games = set->size * set->size;
    size_t start = set->size * set->size / 2;
    size_t chunk_size = 50;
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
    create_partition (&part, pinfo, set->size);

    do
    {
        size_t net_1 = part.start_x;
        size_t net_2 = part.start_y;
        size_t count = 0;

        for (; count < part.len; ++net_1)
        {
            player_t* p1 = player_create_net (set->nets[net_1]);
            for (; net_2 < set->size && count < part.len; ++net_2)
            {
                ++count;

                if (net_1 == net_2)
                    continue;

                player_t* p2 = player_create_net (set->nets[net_2]);
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
    generation_stats_t total_stats = { 0, 0, 0, 0, {0, 0} };
    uint64_t game_cnt = set->size * (set->size - 1);
    struct timespec start, end;
    struct timespec diff;

    /* csv header */
    if (pinfo.mpi_rank == 0 && !opts->human_readable)
        csv_header (stdout);

    create_partition (&part, &pinfo, set->size);
    for (size_t gen = 0; gen < opts->n; ++gen)
    {
        /* reset stats */
        memset (&stats, 0x00, sizeof (generation_stats_t));
        stats.generation = gen;
        stats.game_cnt = game_cnt;

        /* start generation time */
        clock_gettime (CLOCK_MONOTONIC, &start);
        memset (wins, 0x00, set->size * sizeof (uint64_t));

        if (gen)
            the_next_generation (pop);

        if (pinfo.mpi_rank == 0)
            master (&pinfo, set);
        else
            worker (opts, &pinfo, set, wins, &stats);

        MPI_Allreduce (MPI_IN_PLACE, wins, (int) set->size, MPI_UINT64_T,
                       MPI_SUM, MPI_COMM_WORLD);
        for (size_t net = 0; net < set->size; ++net)
            pop->individuals[net]->fitness = (float) wins[net];

        /* end generation time */
        clock_gettime (CLOCK_MONOTONIC, &end);
        diff = diff_timespec (start, end);
        stats.generation_time = sum_timespec (stats.generation_time, diff);

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

    if (pinfo.mpi_rank == 0 && !opts->human_readable)
    {
        fprintf (stdout, "# total: ");
        csv_line (&total_stats, stdout);
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
