#ifndef NGG_GAME_H
#define NGG_GAME_H

/**
 * \file
 * \brief Declares helper functions and data structures for ngg_game.
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 */

#include "neuralnet/neuralnet.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>


/**
 * \brief Contains options set by command line arguments.
 */
typedef struct
{
    char* in_path;
    char* out_path;
    bool b_in;
    bool b_out;

    size_t board_size;

    size_t n;

    unsigned seed;

    bool set_a;
    bool set_i;
    bool set_n;
    bool set_o;
    bool set_s;
    bool set_seed;

    bool verbose;
    bool human_readable;

    bool set_sched_chunksize;
    bool set_sched_initial;
    size_t sched_chunksize;
    double sched_initial;
} options_t;


/**
 * \brief Stores information about the games in one generation.
 */
typedef struct
{
    /** \brief Number of this generation. */
    uint64_t generation;
    /** \brief Number of games played. */
    uint64_t game_cnt;
    /** \brief Number of plays (placed stones). */
    uint64_t play_cnt;
    /** \brief Number of passes. */
    uint64_t pass_cnt;

    /** \brief Time used in this generation. */
    struct timespec generation_time;
} generation_stats_t;


/**
 * \brief Sets all options to zero/false.
 * \param opt options
 * \pre opts != NULL
 */
void init_opts (options_t* opts);

/**
 * \brief Checks whether all required options have been set.
 * \param opts Options.
 * \return 0 if everything is ok, else != 0.
 * \pre opts != NULL
 */
int unsupervised_check_options (options_t* opts);

/**
 * \brief Checks whether the networks in a set have the right layout.
 * \param set Set of neural networks.
 * \param board_size Size of the board to play with.
 * \return 0 if everything is ok, else != 0.
 * \pre set != NULL
 * \pre board_size > 1
 */
int check_board_size (nnet_set_t* set, size_t board_size);

/**
 * \brief Does unsupervised learning on a set of neural networks.
 * \param opts The command line parameter
 * \param argc from main
 * \param argv from main
 * \pre opts != NULL
 */
int unsupervised (options_t* opts, int argc, char** argv);

/**
 * \brief Add the stats of one struct to another (dst += src)
 * \param dst Destination operand.
 * \param src Source operand.
 * \pre dst != NULL
 * \pre src != NULL
 * \post dst'->generation = max({src,dst}->generation)
 */
void stats_add (generation_stats_t* dst, generation_stats_t* src);

/**
 * \brief Prints a generation stats object with explanations.
 * \param stats Stats object.
 * \param file Open, writable file.
 * \pre stats != NULL
 * \pre file != NULL
 */
void human_output (generation_stats_t* stats, FILE* file);

/**
 * \brief Prints a CSV comment (#) explaining the columns accordingly to
 * csv_line().
 * \param file Open, writable file.
 * \pre file != NULL
 */
void csv_header (FILE* file);

/**
 * \brief Prints a generation stats object as CSV line.
 * \param stats Stats object.
 * \param file Open, writable file.
 * \pre stats != NULL
 * \pre file != NULL
 */
void csv_line (generation_stats_t* stats, FILE* file);

#endif /* NGG_GAME_H */
