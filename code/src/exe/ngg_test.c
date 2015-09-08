#include "ngg_test.h"
#include <argp.h>
#include <stdlib.h>
#include <string.h>

#include "ngg_test_impl.c"


static int parse_opt (int key, char* arg, struct argp_state* state)
{
    options_t* opts = state->input;
    switch (key)
    {
    case 'b':  // binary
        opts->b_in = true;
        break;

    case 200:  // binary-in
        opts->b_in = true;
        break;

    case 'i':  // input 1
        if (strlen (arg) > 0)
        {
            opts->in_path1 = arg;
            opts->set_i = true;
        }
        else
        {
            argp_error (state, "input filename is empty");
        }
        break;

    case 'j':  // input 1
        if (strlen (arg) > 0)
        {
            opts->in_path2 = arg;
            opts->set_j = true;
        }
        else
        {
            argp_error (state, "input filename is empty");
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

    case 'v':  // netver1
        {
            unsigned long ver = strtoul (arg, NULL, 10);
            if (ver < 2)
                opts->ver1 = ver;
            break;
        }

    case 'w':  // netver2
        {
            unsigned long ver = strtoul (arg, NULL, 10);
            if (ver < 2)
                opts->ver2 = ver;
            break;
        }
    }
    return 0;
}

int main (int argc, char** argv)
{
    options_t opts;
    init_opts (&opts);

    struct argp_option options[] = {
        {"in1", 'i', "FILE", 0, "load neuralnet from file", 0},
        {"in2", 'j', "FILE", 0, "load neuralnet from file", 0},
        {"binary", 'b', 0, 0, "use binary files", 0},
        {"binary-in", 200, 0, 0, "use binary input file", 0},
        {"board-size", 's', "NUM", 0, "size of the used go board", 0},
        {"netver1", 'v', "0", 0, "nnet1 player version", 0},
        {"netver2", 'w', "0", 0, "nnet2 player version", 0},
        {0, 0, 0, 0, 0, 0}};
    struct argp argp = {options, &parse_opt, 0, 0, 0, 0, 0};
    argp_parse (&argp, argc, argv, 0, 0, &opts);

    int ret = test (&opts);

    return ret;
}
