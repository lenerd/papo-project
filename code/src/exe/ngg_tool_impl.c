#include "ngg_tool.h"
#include "neuralnet/neuralnet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void init_opts (options_t* opts)
{
    memset (opts, 0x00, sizeof (options_t));
}


void cleanup_opts (options_t* opts)
{
    if (opts->number_neurons != NULL)
    {
        free (opts->number_neurons);
        opts->number_neurons = NULL;
    }
}


int create_networks (options_t* opts)
{
    int ret = EXIT_SUCCESS;
    if (!opts->set_l)
    {
        fprintf (stderr, "error: specify network layout with -l\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_o)
    {
        fprintf (stderr, "error: specify output file with -o\n");
        ret = EXIT_FAILURE;
    }
    if (ret)
        return ret;

    neuralnet_t* net =
        nnet_create_random (opts->number_layer, opts->number_neurons);

    nnet_to_file (net, opts->out_path, opts->b_out);

    nnet_destroy (net);
    return ret;
}

int generate_training_data (options_t* opts)
{
    int ret = EXIT_SUCCESS;

    if (!opts->set_n)
    {
        fprintf (stderr, "error: specify size of dataset with -n\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_o)
    {
        fprintf (stderr, "error: specify output file with -o\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_s)
    {
        fprintf (stderr,
                 "error: specify size of go board with -s\n");
        ret = EXIT_FAILURE;
    }
    if (ret)
        return ret;

    return ret;
}

int train_networks (options_t* opts)
{
    int ret = EXIT_SUCCESS;

    if (!opts->set_i)
    {
        fprintf (stderr, "error: specify input file with -i\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_n)
    {
        fprintf (stderr, "error: specify number of iterations with -n\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_o)
    {
        fprintf (stderr, "error: specify output file with -o\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_t)
    {
        fprintf (stderr,
                 "error: specify file containing training data with -t\n");
        ret = EXIT_FAILURE;
    }
    if (ret)
        return ret;

    neuralnet_t* net = nnet_from_file (opts->in_path, opts->b_in);

    nnet_to_file (net, opts->out_path, opts->b_out);

    return ret;
}
