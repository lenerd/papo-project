#include "ngg_game.h"
#include "neuralnet/neuralnet.h"
#include "go/game.h"
#include "go/player.h"
#include "genetic/genetic_algorithm.h"
#include "util/util.h"
#include "util/misc.h"

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


int unsupervised (options_t* opts)
{
    int ret = unsupervised_check_options (opts);
    if (ret)
        return ret;

    /* seed random number generator */
    if (opts->set_seed)
        srand (opts->seed);
    else
        srand ((unsigned int) time (0));

    /* load neural networks */
    nnet_set_t* set = nnet_set_from_file (opts->in_path, opts->b_in);
    ret = check_board_size (set, opts->board_size, opts->ver);
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
    population_t* pop = population_create (set->size, genomes, 1.0f, 0.01f);

    /* fitness values */
    uint64_t* wins = SAFE_MALLOC (set->size * sizeof (uint64_t));

    /* statistic */
    generation_stats_t stats;
    generation_stats_t total_stats = { 0, 0, 0, 0, {0, 0} };
    uint64_t game_cnt = set->size * (set->size - 1);
    struct timespec start, end;
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
        clock_gettime (CLOCK_MONOTONIC, &start);

        for (size_t net_1 = 0; net_1 < set->size; ++net_1)
        {
            player_t* p1 = player_create_net (set->nets[net_1], ver0);

            for (size_t net_2 = 0; net_2 < set->size; ++net_2)
            {
                if (net_1 == net_2)
                    continue;
                player_t* p2 = player_create_net (set->nets[net_2], ver0);
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
                stats.play_cnt += game->play_cnt;
                stats.pass_cnt += game->pass_cnt;

                game_destroy (game);
                player_destroy (p2);
            }

            player_destroy (p1);
        }

        // Make it so!
        the_next_generation (pop);

        /* end generation time */
        clock_gettime (CLOCK_MONOTONIC, &end);
        diff = diff_timespec (start, end);
        stats.generation_time = sum_timespec (stats.generation_time, diff);
        stats_add (&total_stats, &stats);

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

    if (!opts->human_readable)
    {
        fprintf (stdout, "# total: ");
        csv_line (&total_stats, stdout);
    }

    free (wins);
    population_destroy (pop);
    free (genomes);
    nnet_set_to_file (set, opts->out_path, opts->b_out);
    nnet_set_destroy (set);

    return EXIT_SUCCESS;
}
