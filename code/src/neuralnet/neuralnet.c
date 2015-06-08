#include "neuralnet/neuralnet.h"

#include "math_extend/math_ext.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uint32_t edge_count (uint32_t input_count, uint32_t hidden_layer_count,
                     uint32_t neurons_per_hidden_layer, uint32_t output_count)
{
    return (input_count + 1) * neurons_per_hidden_layer +
           (neurons_per_hidden_layer + 1) * neurons_per_hidden_layer *
               (hidden_layer_count - 1) +
           neurons_per_hidden_layer * output_count;
}

/**
 * \brief Builds an uninitialized neural network.
 * \pre input_count > 0
 * \pre hidden_layer_count > 0
 * \pre neurons_per_hidden_layer > 0
 * \pre output_count > 0
 */
static neuralnet_t* allocate_neural_net (uint32_t input_count,
                                         uint32_t hidden_layer_count,
                                         uint32_t neurons_per_hidden_layer,
                                         uint32_t output_count)
{
    neuralnet_t* net = NULL;
    net = malloc (sizeof (neuralnet_t));
    if (net == NULL)
    {
        fprintf (stderr, "malloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    net->input_count = input_count;
    net->hidden_layer_count = hidden_layer_count;
    net->neurons_per_hidden_layer = neurons_per_hidden_layer;

    net->output_count = output_count;
    net->output = calloc (output_count, sizeof (float));
    if (net->output == NULL)
    {
        fprintf (stderr, "calloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    net->edges_count = edge_count (input_count, hidden_layer_count,
                                   neurons_per_hidden_layer, output_count);
    net->edge_buf = malloc (sizeof (float) * (net->edges_count));
    if (net->edge_buf == NULL)
    {
        fprintf (stderr, "malloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    net->input_edges = calloc (input_count + 1, sizeof (float*));
    if (net->input_edges == NULL)
    {
        fprintf (stderr, "calloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }
    net->hidden_edges = calloc (hidden_layer_count - 1, sizeof (float**));
    if (net->hidden_edges == NULL)
    {
        fprintf (stderr, "calloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }
    net->edge_helper_buf = calloc (
        (hidden_layer_count - 1) * (neurons_per_hidden_layer + 1), sizeof (float*));

    if (net->edge_helper_buf == NULL)
    {
        fprintf (stderr, "calloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }
    net->output_edges = calloc (hidden_layer_count + 1, sizeof (float**));
    if (net->output_edges == NULL)
    {
        fprintf (stderr, "calloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    build_pointer(net);

    return net;
}

void build_pointer (neuralnet_t* net)
{
    ptrdiff_t offset_hidden = net->input_count * net->neurons_per_hidden_layer;
    ptrdiff_t offset_output = offset_hidden +
                              (net->hidden_layer_count - 1) *
                                  net->neurons_per_hidden_layer *
                                  net->neurons_per_hidden_layer;

    for (uint32_t i = 0; i < net->input_count + 1; ++i)
    {
        net->input_edges[i] = net->edge_buf + i * (net->neurons_per_hidden_layer);
    }
    for (uint32_t i = 0; i < net->hidden_layer_count - 1; ++i)
    {
        for (uint32_t j = 0; j < net->neurons_per_hidden_layer + 1; ++j)
        {
            net->edge_helper_buf[i * (net->neurons_per_hidden_layer + 1) + j] =
                net->edge_buf + offset_hidden +
                i * (net->neurons_per_hidden_layer + 1) * net->neurons_per_hidden_layer +
                j * (net->neurons_per_hidden_layer + 1);
        }
        net->hidden_edges[i] =
            &net->edge_helper_buf[i * net->neurons_per_hidden_layer];
    }
    for (uint32_t i = 0; i < net->neurons_per_hidden_layer + 1; ++i)
    {
        net->output_edges[i] = net->edge_buf + offset_output + i * net->output_count;
    }
}

void update_neuralnet (void* arg)
{
    neuralnet_t* net = (neuralnet_t*) arg;
    build_pointer (net);
}

/**
 * \brief Initializes a given neuralnet with random edge-weights.
 * \param net Neural network to initialize.
 * \pre net != NULL
 * \post net is initialized.
 */
static void initialize_neural_net_random (neuralnet_t* net)
{
    for (uint32_t i = 0; i < net->edges_count; ++i)
    {
        net->edge_buf[i] = inverse_sigmoid (random_value_01 ());
    }
}

/**
 * \brief Initializes a given neuralnet with given edge-weights in form of an
 * float array.
 * \param net Neural network to initialize.
 * \param edges Buffer containing edge values.
 * \pre net != NULL
 * \pre edges != NULL
 * \pre length(edges) = net->edge_count
 * \post net is initialized.
 */
static void initialize_neural_net_buffer (neuralnet_t* net, float* edges)
{
    memcpy (net->edge_buf, edges, net->edges_count);
}

neuralnet_t* create_neural_net_random (uint32_t input_count,
                                       uint32_t hidden_layer_count,
                                       uint32_t neurons_per_hidden_layer,
                                       uint32_t output_count)
{
    neuralnet_t* net =
        allocate_neural_net (input_count, hidden_layer_count,
                             neurons_per_hidden_layer, output_count);
    initialize_neural_net_random (net);
    return net;
}

neuralnet_t* create_neural_net_buffer (uint32_t input_count,
                                       uint32_t hidden_layer_count,
                                       uint32_t neurons_per_hidden_layer,
                                       uint32_t output_count, float* edges)
{
    neuralnet_t* net =
        allocate_neural_net (input_count, hidden_layer_count,
                             neurons_per_hidden_layer, output_count);
    initialize_neural_net_buffer (net, edges);
    return net;
}

void neural_net_to_file (neuralnet_t* net, const char* path, bool binary)
{
    FILE* file = NULL;
    file = fopen (path, "w");
    if (file == NULL)
    {
        fprintf (stderr, "fopen() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    if (binary)
    {
        fwrite (&net->edges_count, sizeof (net->edges_count), 1, file);
        fwrite (&net->input_count, sizeof (net->input_count), 1, file);
        fwrite (&net->hidden_layer_count, sizeof (net->hidden_layer_count), 1,
                file);
        fwrite (&net->neurons_per_hidden_layer,
                sizeof (net->neurons_per_hidden_layer), 1, file);
        fwrite (&net->output_count, sizeof (net->output_count), 1, file);
        fwrite (net->edge_buf, sizeof (float), net->edges_count, file);
    }
    else
    {
        // TODO
    }

    fclose (file);
}

neuralnet_t* neural_net_from_file (const char* path, bool binary)
{
    uint32_t edges_count = 0;
    uint32_t input_count = 0;
    uint32_t hidden_layer_count = 0;
    uint32_t neurons_per_hidden_layer = 0;
    uint32_t output_count = 0;

    neuralnet_t* net;
    FILE* file = NULL;
    file = fopen (path, "r");
    if (file == NULL)
    {
        fprintf (stderr, "fopen() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    if (binary)
    {
        bool success = true;
        size_t num = 0;
        num = fread (&edges_count, sizeof (edges_count), 1, file);
        success = success && num == 1;
        num = fread (&input_count, sizeof (input_count), 1, file);
        success = success && num == 1;
        num = fread (&hidden_layer_count, sizeof (hidden_layer_count), 1, file);

        num = fread (&neurons_per_hidden_layer,
                     sizeof (neurons_per_hidden_layer), 1, file);
        success = success && num == 1;
        fread (&output_count, sizeof (output_count), 1, file);
        success = success && num == 1;
        if (edges_count != edge_count (input_count, hidden_layer_count,
                                       neurons_per_hidden_layer, output_count))
        {
            fprintf (stderr, "file corrupted in file %s at line # %d", __FILE__,
                     __LINE__);
            fclose (file);
            exit (EXIT_FAILURE);
        }
        net = allocate_neural_net (input_count, hidden_layer_count,
                                   neurons_per_hidden_layer, output_count);
        num = fread (net->edge_buf, sizeof (float), edges_count, file);
        success = success && num == edges_count;

        if (!success)
        {
            if (feof (file))
            {
                fprintf (stderr,
                         "fread reached unexpected EOF in file %s at line # %d",
                         __FILE__, __LINE__);
                fclose (file);
                exit (EXIT_FAILURE);
            }
            if (ferror (file))
            {
                fprintf (stderr,
                         "error occurred in fread in file %s at line # %d",
                         __FILE__, __LINE__);
                fclose (file);
                exit (EXIT_FAILURE);
            }
        }
    }
    else
    {
        // TODO
    }

    fclose (file);

    return net;
}

void destroy_neural_net (neuralnet_t* net)
{
    free (net->input_edges);
    free (net->hidden_edges);
    free (net->output_edges);
    free (net->edge_helper_buf);
    free (net->edge_buf);
    free (net->output);
    free (net);
}

float* calculate_output (neuralnet_t* net, float* input)
{
    /* buffer for intermediary results */
    float* ires1 = NULL;
    float* ires2 = NULL;

    ires1 = malloc (sizeof (float) * net->neurons_per_hidden_layer);
    if (ires1 == NULL)
    {
        fprintf (stderr, "malloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    ires2 = malloc (sizeof (float) * net->neurons_per_hidden_layer);
    if (ires2 == NULL)
    {
        fprintf (stderr, "malloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    float sum = 0.0f;

    /* Input layer to hidden layer calculation */
    for (uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to)
    {
        /* threshold */
        sum = -1 * net->input_edges[net->input_count][to];
        for (uint32_t from = 0; from < net->input_count; ++from)
        {
            sum += net->input_edges[from][to] * input[from];
        }
        ires2[to] = (sum > 0.0f) ? centered_sigmoid (sum) : 0.0f;
    }

    /* between hidden layers */
    for (uint32_t layer = 0; layer < net->hidden_layer_count - 1; ++layer)
    {
        /* swap buffers */
        float* tmp = ires1;
        ires1 = ires2;
        ires2 = tmp;

        for (uint32_t to = 0; to < net->neurons_per_hidden_layer; to++)
        {
            /* threshold */
            sum = -1 *
                  net->hidden_edges[layer][net->neurons_per_hidden_layer][to];
            for (uint32_t from = 0; from < net->neurons_per_hidden_layer;
                 ++from)
            {
                sum += net->hidden_edges[layer][from][to] * ires1[from];
            }
            ires2[to] = (sum > 0.0f) ? centered_sigmoid (sum) : 0.0f;
        }
    }

    /* swap buffers */
    {
        float* tmp = ires1;
        ires1 = ires2;
        ires2 = tmp;
    }

    /* hidden layer to output layer calculation */
    for (uint32_t to = 0; to < net->output_count; ++to)
    {
        sum = 0.0f;
        for (uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from)
        {
            sum += net->output_edges[from][to] * ires1[from];
        }
        net->output[to] = sum;
    }

    free (ires1);
    free (ires2);

    return net->output;
}

// void print_neural_net(const neuralnet_t* net){
// 
// 	int index = 0;
// 
// 	printf("Neural Net Edge Weights:");
// 
// 	//Input layer to hidden layer calculation
// 	printf("\n\n\tInput layer to hidden layer:\n");
// 	for (uint32_t i = 0; i < net->neurons_per_hidden_layer; i++){
// 		//Threashold and incoming edge weights of i-th neuron
// 		printf("\n\t\tTH: %+.2f EWs:", net->edges[index]);
// 		index++;
// 		for (uint32_t j = 0; j < net->input_count; j++, index++){
// 			printf(" %+.2f", net->edges[index]);
// 		}
// 	}
// 
// 	//Hidden layer intern calculation
// 	printf("\n\n\tHidden layer intern:");
// 	for (uint32_t i = 0; i < net->hidden_layer_count - 1; i++){
// 		printf("\n");
// 		for (uint32_t j = 0; j < net->neurons_per_hidden_layer; j++){
// 			//Threashold and incoming edge weights of j-th neuron
// 			printf("\n\t\tTH: %+.2f EWs:", net->edges[index]);
// 			index++;
// 			for (uint32_t k = 0; k < net->neurons_per_hidden_layer; k++, index++){
// 				printf(" %+.2f", net->edges[index]);
// 			}
// 		}
// 	}
// 
// 	//Hidden layer to output layer
// 	printf("\n\n\tHidden layer to output layer:\n");
// 	for (uint32_t i = 0; i < net->output_count; i++){
// 		//Threashold and incoming edge weights of i-th neuron
// 		printf("\n\t\tEWs:");
// 		for (uint32_t j = 0; j < net->neurons_per_hidden_layer; j++, index++){
// 			printf(" %+.2f", net->edges[index]);
// 		}
// 	}
// 	printf("\n\n\n");
// 
// }
