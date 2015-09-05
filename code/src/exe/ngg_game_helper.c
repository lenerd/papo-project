#include "ngg_game.h"
#include "neuralnet/neuralnet.h"
#include "util/util.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_opts (options_t* opts)
{
    assert (opts != NULL);

    memset (opts, 0x00, sizeof (options_t));
}

int unsupervised_check_options (options_t* opts)
{
    assert (opts != NULL);

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
    if (!opts->set_sched_chunksize)
        opts->sched_chunksize = 1;
    if (!opts->set_sched_initial)
        opts->sched_initial = 0;
    return ret;
}

void stats_add (generation_stats_t* dst, generation_stats_t* src)
{
    assert (dst != NULL);
    assert (src != NULL);

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
    assert (stats != NULL);
    assert (file != NULL);

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
             (double) stats->game_cnt /
                 timespec_to_double (stats->generation_time));
    fprintf (file, "moves: %" PRIu64 "\n", move_cnt);
    fprintf (file, "moves/s: %f\n",
             (double) move_cnt / timespec_to_double (stats->generation_time));
    fprintf (file, "moves/game: %f\n",
             (double) move_cnt / (double) stats->game_cnt);
    fprintf (file, "plays: %" PRIu64 "\n", stats->play_cnt);
    fprintf (file, "passes: %" PRIu64 "\n", stats->pass_cnt);
    fprintf (file, "passes/s: %f\n",
             (double) stats->pass_cnt /
                 timespec_to_double (stats->generation_time));
    fprintf (file, "passes/game: %f\n",
             (double) stats->pass_cnt / (double) stats->game_cnt);
    fprintf (file, "passes/moves: %f\n",
             (double) stats->pass_cnt / (double) move_cnt);
}

void csv_header (FILE* file)
{
    assert (file != NULL);

    fprintf (file, "# generation,generation time,game count,play "
                   "count,pass count\n");
}

void csv_line (generation_stats_t* stats, FILE* file)
{
    assert (stats != NULL);
    assert (file != NULL);

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
