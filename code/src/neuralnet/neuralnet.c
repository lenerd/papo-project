#include "neuralnet.h"
#include "util/util.h"
#include "util/math_ext.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Construction and Destruction ########################################
//
static void skip_comments (FILE* file);

nnet_set_t* nnet_set_create (size_t size)
{
    assert (size > 0);

    nnet_set_t* set = SAFE_CALLOC(1, sizeof(nnet_set_t));
    set->size = size;
    set->nets = SAFE_CALLOC(size, sizeof(neuralnet_t*));

    return set;
}

void nnet_set_destroy (nnet_set_t* set)
{
    assert (set != NULL);

    for (size_t i = 0; i < set->size; ++i)
        if (set->nets[i] != NULL)
            nnet_destroy(set->nets[i]);

    free (set->nets);
    free (set);
}


void nnet_set_to_file (const nnet_set_t* set, const char* path, bool binary)
{
    assert (set != NULL);
    assert (path != NULL);

    FILE* file = NULL;
    file = fopen (path, "w");
    if (file == NULL)
    {
        fprintf (stderr, "fopen() failed in file %s at line # %d\n", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    if (binary)
    {
        fwrite (&set->size, sizeof(set->size), 1, file);
        for (size_t i = 0; i < set->size; ++i)
        {
            if (set->nets[i] == NULL)
                continue;
            nnet_to_file (set->nets[i], file, true);
        }
    }
    else
    {
        fprintf (file, "# nnet_set_t\n");
        fprintf (file, "size = %zu\n", set->size);
        fprintf (file, "#\n");
        for (size_t i = 0; i < set->size; ++i)
        {
            if (set->nets[i] == NULL)
                continue;
            fprintf (file, "# neuralnet #%zu\n", i);
            nnet_to_file (set->nets[i], file, false);
            fprintf (file, "#\n");
        }
    }

    fclose (file);
}

nnet_set_t* nnet_set_from_file (const char* path, bool binary)
{
    assert (path != NULL);

    size_t size = 0;

    nnet_set_t* set;
    FILE* file = NULL;
    file = fopen (path, "r");
    if (file == NULL)
    {
        fprintf (stderr, "fopen() failed in file %s at line # %d\n", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    if (binary)
    {
        bool success = true;
        size_t num = 0;

        num = fread (&size, sizeof (size), 1, file);
        success = success && num == 1;

        set = nnet_set_create (size);

        for (size_t i = 0; i < size; ++i)
        {
            neuralnet_t* net = nnet_from_file (file, true);
            if (net != NULL)
                set->nets[i] = net;
        }
    }
    else
    {
        skip_comments (file);
        fscanf (file, "size = %zu\n", &size);

        set = nnet_set_create (size);

        for (size_t i = 0; i < size; ++i)
        {
            skip_comments (file);
            neuralnet_t* net = nnet_from_file (file, false);
            if (net != NULL)
                set->nets[i] = net;
        }
    }

    fclose (file);

    return set;
}

size_t nnet_edge_count (size_t layer_count, const size_t* neurons_per_layer)
{
    size_t cnt = 0;
    for (size_t i = 0; i < layer_count - 1; ++i)
        cnt += neurons_per_layer[i] * neurons_per_layer[i + 1];
    return cnt;
}

size_t nnet_node_count (size_t layer_count, const size_t* neurons_per_layer)
{
    size_t cnt = 0;
    for (size_t i = 0; i < layer_count; ++i)
        cnt += neurons_per_layer[i];
    return cnt;
}

void build_pointer (neuralnet_t* net)
{
    assert (net != NULL);

    float* current_gap = net->edge_buf;

    size_t cnt_from = 0;
    for (size_t gap = 0; gap < net->layer_count - 1; ++gap)
    {
        net->edges[gap] = net->edge_helper + cnt_from;
        size_t cnt_to = net->neurons_per_layer[gap + 1];
        for (size_t from = 0; from < net->neurons_per_layer[gap]; ++from)
        {
            net->edge_helper[cnt_from] = current_gap + from * cnt_to;
            ++cnt_from;
        }
        current_gap +=
            net->neurons_per_layer[gap] * net->neurons_per_layer[gap + 1];
    }
}

static neuralnet_t* allocate_neural_net (const size_t layer_count,
                                         const size_t* neurons_per_layer)
{
    assert (layer_count > 1);
    assert (neurons_per_layer != NULL);

    neuralnet_t* net = NULL;
    net = SAFE_MALLOC (sizeof (neuralnet_t));
    net->neurons_per_layer = SAFE_MALLOC (layer_count * sizeof (size_t));

    size_t edge_cnt =
        nnet_edge_count (layer_count, neurons_per_layer);
    size_t help_len =
        nnet_node_count (layer_count, neurons_per_layer) -
        neurons_per_layer[layer_count - 1];

    net->layer_count = layer_count;
    net->edge_count = edge_cnt;
    memcpy (net->neurons_per_layer, neurons_per_layer,
            layer_count * sizeof (size_t));

    net->edge_buf = SAFE_CALLOC (edge_cnt, sizeof (float));
    net->edge_helper = SAFE_CALLOC (help_len, sizeof (float*));
    net->edges = SAFE_CALLOC (net->layer_count - 1, sizeof (float**));

    build_pointer (net);

    return net;
}

void init_neural_net (neuralnet_t* net)
{
    for (size_t gap = 0; gap < net->layer_count - 1; ++gap)
        for (size_t from = 0; from < net->neurons_per_layer[gap]; ++from)
            for (size_t to = 0; to < net->neurons_per_layer[gap + 1]; ++to)
                net->edges[gap][from][to] = random_value_mm (-1.0f, 1.0f);
}


neuralnet_t* nnet_create_random (const size_t layer_count,
                                       const size_t* neurons_per_layer)
{
    assert (layer_count > 1);
    assert (neurons_per_layer != NULL);

    neuralnet_t* net = allocate_neural_net (layer_count, neurons_per_layer);
    init_neural_net (net);

    return net;
}

neuralnet_t* nnet_create_buffer (const size_t layer_count,
                                       const size_t* neurons_per_layer,
                                       const float* edges)
{
    assert (layer_count > 1);
    assert (neurons_per_layer != NULL);
    assert (edges != NULL);

    neuralnet_t* net = allocate_neural_net (layer_count, neurons_per_layer);
    memcpy (net->edge_buf, edges,
            nnet_edge_count (layer_count, neurons_per_layer) * sizeof (float));

    return net;
}

void nnet_to_file (const neuralnet_t* net, FILE* file, bool binary)
{
    assert (net != NULL);
    assert (file != NULL);

    if (binary)
    {
        fwrite (&net->edge_count, sizeof (net->edge_count), 1, file);
        fwrite (&net->layer_count, sizeof (net->layer_count), 1, file);
        fwrite (net->neurons_per_layer,
                sizeof (size_t), net->layer_count, file);
        fwrite (net->edge_buf, sizeof (float), net->edge_count, file);
    }
    else
    {
        fprintf (file, "# neuralnet_t\n");
        fprintf (file, "edge_count = %zu\n", net->edge_count);
        fprintf (file, "layer_count = %zu\n", net->layer_count);
        fprintf (file, "# neurons_per_layer\n");
        for (size_t i = 0; i < net->layer_count; ++i)
        {
            fprintf(file, "%zu ", net->neurons_per_layer[i]);
        }

        fprintf (file, "\n");
        for (size_t gap = 0; gap < net->layer_count - 1; ++gap)
        {
            fprintf (file, "# edges from layer %zu to %zu\n", gap, gap + 1);
            for (uint32_t from = 0; from < net->neurons_per_layer[gap];
                 ++from)
            {
                for (uint32_t to = 0; to < net->neurons_per_layer[gap+1]; ++to)
                {
                    fprintf (file, "%+.9e ",
                             (double) net->edges[gap][from][to]);
                }
                fprintf (file, "\n");
            }
        }
    }
}

static void skip_comments (FILE* file)
{
    bool comment = true;
    while (comment)
    {
        int c = fgetc (file);
        if (c == '#')
        {
            while (c != '\n')
            {
                c = fgetc (file);
                if (c == EOF)
                {
                    ungetc (EOF, file);
                    return;
                }
            }
        }
        else
        {
            ungetc (c, file);
            comment = false;
        }
    }
}

neuralnet_t* nnet_from_file (FILE* file, bool binary)
{
    assert (file != NULL);

    size_t edge_count = 0;
    size_t layer_count = 0;
    size_t* neurons_per_layer = NULL;

    neuralnet_t* net;

    if (binary)
    {
        bool success = true;
        size_t num = 0;

        num = fread (&edge_count, sizeof (edge_count), 1, file);
        success = success && num == 1;

        num = fread (&layer_count, sizeof (layer_count), 1, file);
        success = success && num == 1;

        neurons_per_layer = SAFE_MALLOC (layer_count * sizeof (size_t));

        num = fread (neurons_per_layer, sizeof (size_t), layer_count, file);
        success = success && num == layer_count;

        if (edge_count != nnet_edge_count (layer_count, neurons_per_layer))
        {
            fprintf (stderr, "edge_count %zu nnet_edge_count %zu\n", edge_count, nnet_edge_count(layer_count, neurons_per_layer));
            fprintf (stderr, "file corrupted in file %s at line # %d\n",
                     __FILE__, __LINE__);
            free (neurons_per_layer);
            return NULL;
        }

        net = allocate_neural_net (layer_count, neurons_per_layer);
        free (neurons_per_layer);
        neurons_per_layer = NULL;

        num = fread (net->edge_buf, sizeof (float), edge_count, file);
        success = success && num == edge_count;

        if (!success)
        {
            const char* line = __LINE__;
            const char* filename = __FILE__;
            if (feof (file))
            {
                fprintf (
                    stderr,
                    "fread reached unexpected EOF in file %s at line # %d\n",
                    filename, line);
            }
            else if (ferror (file))
            {
                fprintf (stderr,
                         "error occurred in fread in file %s at line # %d",
                    filename, line);
            }
            else
            {
                fprintf (stderr,
                         "unknown error occurred in fread in file %s at line # %d",
                    filename, line);
            }
            nnet_destroy (net);
            return NULL;
        }
    }
    else
    {
        skip_comments (file);
        fscanf (file, "edge_count = %zu\n", &edge_count);

        skip_comments (file);
        fscanf (file, "layer_count = %zu\n", &layer_count);

        neurons_per_layer = SAFE_MALLOC (layer_count * sizeof (size_t));

        skip_comments (file);
        fscanf (file, "neurons_per_layer\n");

        for (size_t i = 0; i < layer_count; ++i)
        {
            fscanf (file, "%zu ", neurons_per_layer + i);
        }

        if (edge_count != nnet_edge_count (layer_count, neurons_per_layer))
        {
            fprintf (stderr, "file corrupted in file %s at line # %d\n",
                     __FILE__, __LINE__);
            free (neurons_per_layer);
            return NULL;
        }

        net = allocate_neural_net (layer_count, neurons_per_layer);

        for (size_t gap = 0; gap < layer_count - 1; ++gap)
        {
            skip_comments (file);
            for (size_t from = 0; from < neurons_per_layer[gap]; ++from)
            {
                for (size_t to = 0; to < neurons_per_layer[gap+1]; ++to)
                {
                    fscanf (file, "%e ", &net->edges[gap][from][to]);
                }
            }
            fscanf (file, "\n");
        }
    }

    free (neurons_per_layer);

    return net;
}

void nnet_destroy (neuralnet_t* net)
{
    assert (net != NULL);

    free (net->edges);
    free (net->edge_helper);
    free (net->edge_buf);

    free (net->neurons_per_layer);

    free (net);
}

// Calculation #########################################################

float* nnet_calculate_output (const neuralnet_t* net, const float* input)
{
    assert (net != NULL);
    assert (input != NULL);

    size_t max = max_size (net->neurons_per_layer, net->layer_count);
    float* current_result_1 = SAFE_MALLOC (max * sizeof (float));
    float* current_result_2 = SAFE_MALLOC (max * sizeof (float));

    memcpy (current_result_1, input,
            net->neurons_per_layer[0] * sizeof (float));
    sigmoidize_inplace (current_result_1, net->neurons_per_layer[0]);

    for (size_t gap = 0; gap < net->layer_count - 1; ++gap)
    {
        for (size_t to = 0; to < net->neurons_per_layer[gap + 1]; ++to)
        {
            current_result_2[to] = 0.0f;

            for (size_t from = 0; from < net->neurons_per_layer[gap]; ++from)
            {
                current_result_2[to] +=
                    current_result_1[from] * net->edges[gap][from][to];
            }

            current_result_2[to] = sigmoid (current_result_2[to]);
        }

        swap_float_buffer (&current_result_1, &current_result_2);
    }
    free (current_result_2);

    return desigmoidize_inplace (current_result_1,
                                 net->neurons_per_layer[net->layer_count - 1]);
}


void nnet_print (const neuralnet_t* net){
    assert (net != NULL);

	printf("\nNeural network at Adress x:");

	for(uint32_t gap = 0; gap < net->layer_count - 1; ++gap){

		printf("\n\tGap %d:", gap);

		for(uint32_t to = 0; to < net->neurons_per_layer[gap + 1]; ++to){

			printf("\n\t\tTo Neuron %d:", to);

			for(uint32_t from = 0; from < net->neurons_per_layer[gap]; ++from){

				printf(" %f", (double)net->edges[gap][from][to]);

			}

		}

	}

	printf("\n");

}
