#include "ngg_game.h"
#include "neuralnet/neuralnet.h"
#include "go/game.h"
#include "go/player.h"
#include "genetic/genetic_algorithm.h"
#include "util/util.h"

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
    struct timespec game_time;
} generation_stats_t;


void human_output (generation_stats_t* stats, FILE* file)
{
    uint64_t move_cnt = stats->play_cnt + stats->pass_cnt;

    /* print stats */
    fprintf (file, "==============\n");
    fprintf (file, "generation %zu\n\n", stats->generation);
    fprintf (file, "generation time: ");
    print_time (stats->generation_time, file);
    fputc ('\n', file);
    fprintf (file, "game time: ");
    print_time (stats->game_time, file);
    fputc ('\n', file);
    fprintf (file, "games: %" PRIu64 "\n", stats->game_cnt);
    fprintf (file, "games/s: %f\n",
             (double) stats->game_cnt / timespec_to_double (stats->game_time));
    fprintf (file, "moves: %" PRIu64 "\n", move_cnt);
    fprintf (file, "moves/s: %f\n",
             (double) move_cnt / timespec_to_double (stats->game_time));
    fprintf (file, "moves/game: %f\n",
             (double) move_cnt / (double) stats->game_cnt);
    fprintf (file, "plays: %" PRIu64 "\n", stats->play_cnt);
    fprintf (file, "passes: %" PRIu64 "\n", stats->pass_cnt);
    fprintf (file, "passes/s: %f\n",
             (double) stats->pass_cnt / timespec_to_double (stats->game_time));
    fprintf (file, "passes/game: %f\n",
             (double) stats->pass_cnt / (double) stats->game_cnt);
    fprintf (file, "passes/moves: %f\n",
             (double) stats->pass_cnt / (double) move_cnt);
}

void csv_header (FILE* file)
{
    fprintf (file, "# generation \\t generation time \\t game time \\t game "
                   "count \\t play "
                   "count \\t pass count\n");
}

void csv_line (generation_stats_t* stats, FILE* file)
{
    char delimiter = ',';
    fprintf (file, "%" PRId64, stats->generation);
    fputc (delimiter, file);
    fprintf (file, "%f", timespec_to_double (stats->generation_time));
    fputc (delimiter, file);
    fprintf (file, "%f", timespec_to_double (stats->game_time));
    fputc (delimiter, file);
    fprintf (file, "%" PRId64, stats->game_cnt);
    fputc (delimiter, file);
    fprintf (file, "%" PRId64, stats->play_cnt);
    fputc (delimiter, file);
    fprintf (file, "%" PRId64, stats->pass_cnt);
    putchar ('\n');
}


int unsupervised (options_t* opts, int argc, char** argv)
{
    int ret = unsupervised_check_options (opts);
    if (ret)
        return ret;

    int rc;

    rc = MPI_Init (&argc, &argv);
    if (rc != MPI_SUCCESS)
    {
        fprintf (stderr, "[%s:%u] MPI_Init failed (%d)\n", __FILE__, __LINE__,
                 rc);
        return rc;
    }

    /* seed random number generator */
    srand ((unsigned int) time (0));

    /* load neural networks */
    nnet_set_t* set = nnet_set_from_file (opts->in_path, opts->b_in);
    ret = check_board_size (set, opts->board_size);
    if (ret)
    {
        nnet_set_destroy (set);
        return ret;
    }

    /* prepare population for the genetic algorithm */
    genome_t** genomes = SAFE_MALLOC (set->size * sizeof (genome_t*));
    for (size_t i = 0; i < set->size; ++i)
        genomes[i] =
            genome_create (set->nets[i]->edge_count, &set->nets[i]->edge_buf,
                           &update_neuralnet, set->nets[i]);
    population_t* pop = population_create (set->size, genomes, 1.0f);

    /* fitness values */
    uint64_t* wins = SAFE_MALLOC (set->size * sizeof (uint64_t));

    /* statistic */
    generation_stats_t stats;
    uint64_t game_cnt = set->size * (set->size - 1);
    struct timespec start, end;
    struct timespec start2, end2;
    struct timespec diff;

    /* csv header */
    if (!opts->human_readable)
        csv_header (stdout);

    for (size_t gen = 0; gen < opts->n; ++gen)
    {
        /* reset stats */
        memset (wins, 0x00, set->size * sizeof (uint64_t));
        memset (&stats, 0x00, sizeof (generation_stats_t));
        stats.generation = gen;
        stats.game_cnt = game_cnt;

        /* start generation time */
        clock_gettime (CLOCK_MONOTONIC, &start2);

        if (gen)
        {
            the_next_generation (pop);
        }

        for (size_t net_1 = 0; net_1 < set->size; ++net_1)
        {
            player_t* p1 = player_create_net (set->nets[net_1]);

            for (size_t net_2 = 0; net_2 < set->size; ++net_2)
            {
                if (net_1 == net_2)
                    continue;
                player_t* p2 = player_create_net (set->nets[net_2]);
                game_t* game = game_create (p1, p2, opts->board_size, 1024);

                /* start game_time */
                clock_gettime (CLOCK_MONOTONIC, &start);

                /* play */
                while (!game->finished)
                    game_step (game);

                /* end game_time */
                clock_gettime (CLOCK_MONOTONIC, &end);
                diff = diff_timespec (start, end);
                stats.game_time = sum_timespec (stats.game_time, diff);

                /* update fitness */
                int64_t score = game_score (game);
                if (score > 0)
                    ++wins[net_1];
                else if (score < 0)
                    ++wins[net_2];

                /* update generation stats */
                stats.play_cnt += game->play_cnt;
                stats.pass_cnt += game->pass_cnt;

                game_destroy (game);
                player_destroy (p2);
            }

            player_destroy (p1);
        }

        /* end generation time */
        clock_gettime (CLOCK_MONOTONIC, &end2);
        diff = diff_timespec (start2, end2);
        stats.generation_time = sum_timespec (stats.generation_time, diff);

        for (size_t net = 0; net < set->size; ++net)
            pop->individuals[net]->fitness = (float) wins[net];

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

    free (wins);
    population_destroy (pop);
    free (genomes);
    nnet_set_to_file (set, opts->out_path, opts->b_out);
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
