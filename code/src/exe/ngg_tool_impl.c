#include "ngg_tool.h"
#include "neuralnet/neuralnet.h"
#include "training/training.h"

#include <math.h>
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
    if (!opts->set_n)
    {
        fprintf (stderr,
                 "error: specify count of networks to create with -n\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_o)
    {
        fprintf (stderr, "error: specify output file with -o\n");
        ret = EXIT_FAILURE;
    }
    if (ret)
        return ret;

    nnet_set_t* set = nnet_set_create (opts->n);
    for (size_t i = 0; i < opts->n; ++i)
    {
        set->nets[i] =
            nnet_create_random (opts->number_layer, opts->number_neurons);
    }

    nnet_set_to_file (set, opts->out_path, opts->b_out);

    nnet_set_destroy (set);
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
        fprintf (stderr, "error: specify size of go board with -s\n");
        ret = EXIT_FAILURE;
    }
    if (ret)
        return ret;

    dataset_t* set = dataset_create (opts->n);

    for (size_t i = 0; i < opts->n; ++i)
        set->data[i] = td_generate_nxn_nxnp1 (opts->board_size);

    dataset_to_file (set, opts->out_path);

    dataset_destroy (set);

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

    nnet_set_t* nets = nnet_set_from_file (opts->in_path, opts->b_in);
    dataset_t* data = dataset_from_file (opts->training_data_path);

    for (size_t i = 0; i < nets->size; ++i)
    {
        if (opts->verbose)
            printf("begin with training of net %zu\n", i);
        for (size_t j = 0; j < opts->n; ++j)
        {
            for (size_t k = 0; k < data->size; ++k)
            {
                if (nets->nets[i]->neurons_per_layer[0] ==
                        data->data[k]->input_size &&
                    nets->nets[i]
                            ->neurons_per_layer[nets->nets[i]->layer_count - 1] ==
                        data->data[k]->output_size)
                {
                    nnet_backpropagate (nets->nets[i], data->data[k]->input,
                                        data->data[k]->expected);
                }
                else
                {
                    fprintf (stderr, "input/output sizes of dataset does not match "
                                     "i/o neurons\n");
                }
            }
        }
    }
    if (opts->verbose)
        printf("finished training\n");

    nnet_set_to_file (nets, opts->out_path, opts->b_out);

    nnet_set_destroy (nets);
    dataset_destroy (data);

    return ret;
}

int calculate (options_t* opts)
{
    int ret = EXIT_SUCCESS;

    if (!opts->set_i)
    {
        fprintf (stderr, "error: specify input file with -i\n");
        ret = EXIT_FAILURE;
    }
    if (!opts->set_t)
    {
        fprintf (stderr,
                 "error: specify file containing input data with -t\n");
        ret = EXIT_FAILURE;
    }
    if (ret)
        return ret;

    nnet_set_t* nets = nnet_set_from_file (opts->in_path, opts->b_in);
    dataset_t* data = dataset_from_file (opts->training_data_path);

    neuralnet_t* net = nets->nets[0];
    training_data_t* td = data->data[0];


    float* output = nnet_calculate_output (net, td->input);

    size_t board_size = (size_t)sqrt((double)td->input_size);

    for (size_t i = 0; i < board_size; ++i)
    {
        for (size_t j = 0; j < board_size; ++j)
        {
            printf("%f ", (double)td->input[i * board_size + j]);
        }
        putchar('\n');
    }

    putchar('\n');

    for (size_t i = 0; i < board_size; ++i)
    {
        for (size_t j = 0; j < board_size; ++j)
        {
            printf("%f ", (double)output[i * board_size + j]);
        }
        putchar('\n');
    }


    nnet_set_destroy (nets);
    dataset_destroy (data);

    return ret;
}
