#include "ngg_tool.h"
#include "neuralnet/neuralnet.h"
#include "util/util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <argp.h>


static int parse_opt (int key, char* arg, struct argp_state* state)
{
    options_t* opts = state->input;
    switch (key)
    {
    case 'a':  // action
        if (strcmp (arg, "create") == 0)
        {
            opts->action = create;
            opts->set_a = true;
        }
        else if (strcmp (arg, "gen-data") == 0)
        {
            opts->action = gen_data;
            opts->set_a = true;
        }
        else if (strcmp (arg, "train") == 0)
        {
            opts->action = train;
            opts->set_a = true;
        }
        else
        {
            argp_error (state, "action not in {create,gen-data,train}");
        }
        break;

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

    case 'l':  // neuralnet layout
    {
        size_t buf_size = 10;
        size_t num_read = 0;
        opts->number_neurons = SAFE_MALLOC (buf_size * sizeof (size_t));
        char* p = strtok (arg, " ");
        while (p)
        {
            size_t tmp = strtoul (p, NULL, 10);
            if (!tmp)
                break;
            ++num_read;
            if (num_read > buf_size)
            {
                buf_size += 10;
                opts->number_neurons = SAFE_REALLOC (
                    opts->number_neurons, buf_size * sizeof (size_t));
            }
            opts->number_neurons[num_read - 1] = tmp;
            p = strtok (0, " ");
        }
        opts->number_layer = num_read;
        if (num_read > 1)
        {
            opts->set_l = true;
        }
        else
        {
            argp_error (state, "requires at least two nonempty layers");
        }
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

    case 't':  // training-data
        if (strlen (arg) > 0)
        {
            opts->training_data_path = arg;
            opts->set_t = true;
        }
        else
        {
            argp_error (state, "training data filename is empty");
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
         "train: trains a neural network with supervised learning",
         0},
        {"in", 'i', "FILE", 0, "load neuralnet from file", 0},
        {"out", 'o', "FILE", 0, "output neuralnet to file", 0},
        {"binary", 'b', 0, 0, "use binary files", 0},
        {"binary-in", 200, 0, 0, "use binary input file", 0},
        {"binary-out", 201, 0, 0, "use binary output file", 0},
        {"number", 'n', "NUM", 0,
         "number of networks to create or training iterations", 0},
        {"board-size", 's', "NUM", 0, "size of the used go board", 0},
        {"training-data", 't', "FILE", 0, "training data to use", 0},
        {"layer", 'l', "NUMS", 0, "number of neurons in each layer\n"
                                  "e.g. \"2 3 3 2\"",
         0},
        {"verbose", 'v', 0, 0, "more prints more information", 0},
        {0, 0, 0, 0, 0, 0}};
    struct argp argp = {options, &parse_opt, 0, 0, 0, 0, 0};
    argp_parse (&argp, argc, argv, 0, 0, &opts);

    int ret = EXIT_SUCCESS;
    switch (opts.action)
    {
    case create:
        ret = create_networks (&opts);
        break;
    case gen_data:
        ret = generate_training_data (&opts);
        break;
    case train:
        ret = train_networks (&opts);
        break;
    default:
        break;
    }
    cleanup_opts (&opts);
    return ret;
}
