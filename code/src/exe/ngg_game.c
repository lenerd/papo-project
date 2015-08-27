#include "ngg_game.h"
#include <argp.h>
#include <stdlib.h>
#include <string.h>


static int parse_opt (int key, char* arg, struct argp_state* state)
{
    options_t* opts = state->input;
    switch (key)
    {
    case 'b':  // binary
        opts->b_in = true;
        opts->b_out = true;
        break;

    case 200:  // binary-in
        opts->b_in = true;
        break;

    case 201:  // binary-out
        opts->b_out = true;
        break;

    case 'h':  // human readable
        opts->human_readable = true;
        break;

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

    case 'n':  // number
        opts->n = strtoul (arg, NULL, 10);
        opts->set_n = true;
        break;

    case 'o':  // output
        if (strlen (arg) > 0)
        {
            opts->out_path = arg;
            opts->set_o = true;
        }
        else
        {
            argp_error (state, "output filename is empty");
        }
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
    }
    break;

    case 'v':  // verbose
        opts->verbose = true;
    }
    return 0;
}

int main (int argc, char** argv)
{
    options_t opts;
    init_opts (&opts);

    struct argp_option options[] = {
        {"action", 'a', "STRING", 0,
         "create: creates a new neural network              "
         "gen-data: generates training data                 "
         "train: trains a neural network with supervised    "
         "learning                                          "
         "calc: calculates output of a neural network given input",
         0},
        {"in", 'i', "FILE", 0, "load neuralnet from file", 0},
        {"out", 'o', "FILE", 0, "output neuralnet to file", 0},
        {"binary", 'b', 0, 0, "use binary files", 0},
        {"binary-in", 200, 0, 0, "use binary input file", 0},
        {"binary-out", 201, 0, 0, "use binary output file", 0},
        {"number", 'n', "NUM", 0,
         "number of generations", 0},
        {"board-size", 's', "NUM", 0, "size of the used go board", 0},
        {"verbose", 'v', 0, 0, "more prints more information", 0},
        {"human-readable", 'h', 0, 0, "human readable output, no csv", 0},
        {0, 0, 0, 0, 0, 0}};
    struct argp argp = {options, &parse_opt, 0, 0, 0, 0, 0};
    argp_parse (&argp, argc, argv, 0, 0, &opts);

    int ret = unsupervised (&opts, argc, argv);

    return ret;
}
