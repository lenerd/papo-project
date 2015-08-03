#include "newneunet.h"
#include "util/util.h"
#include "util/math_ext.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Construction and Destruction ########################################

size_t edge_count (size_t layer_count, const size_t* neurons_per_layer)
{
    size_t cnt = 0;
    for (size_t i = 0; i < layer_count - 1; ++i)
        cnt += neurons_per_layer[i] * neurons_per_layer[i + 1];
    return cnt;
}

size_t node_count (size_t layer_count, const size_t* neurons_per_layer)
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

    net->layer_count = layer_count;
    memcpy (net->neurons_per_layer, neurons_per_layer,
            layer_count * sizeof (size_t));

    size_t edge_cnt = edge_count (net->layer_count, net->neurons_per_layer);
    size_t help_len = node_count (net->layer_count, net->neurons_per_layer) -
                      net->neurons_per_layer[net->layer_count - 1];

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


neuralnet_t* create_neural_net_random (const size_t layer_count,
                                       const size_t* neurons_per_layer)
{
    assert (layer_count > 1);
    assert (neurons_per_layer != NULL);

    neuralnet_t* net = allocate_neural_net (layer_count, neurons_per_layer);
    init_neural_net (net);

    return net;
}

neuralnet_t* create_neural_net_buffer (const size_t layer_count,
                                       const size_t* neurons_per_layer,
                                       const float* edges)
{
    assert (layer_count > 1);
    assert (neurons_per_layer != NULL);
    assert (edges != NULL);

    neuralnet_t* net = allocate_neural_net (layer_count, neurons_per_layer);
    memcpy (net->edge_buf, edges,
            edge_count (layer_count, neurons_per_layer) * sizeof (float));

    return net;
}

void destroy_neural_net (neuralnet_t* net)
{
    assert (net != NULL);

    free (net->edges);
    free (net->edge_helper);
    free (net->edge_buf);

    free (net->neurons_per_layer);

    free (net);
}

// Calculation #########################################################
static float* sigmoidize (const float* array, const size_t size)
{
    float* result = SAFE_MALLOC (size * sizeof (float));

    for (uint32_t i = 0; i < size; ++i)
    {
        result[i] = sigmoid (array[i]);
    }

    return result;
}

static float* desigmoidize (const float* array, const size_t size)
{
    float* result = SAFE_MALLOC (size * sizeof (float));

    for (uint32_t i = 0; i < size; ++i)
    {
        result[i] = inverse_sigmoid (array[i]);
    }

    return result;
}

static float* sigmoidize_inplace (float* array, const size_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        array[i] = sigmoid (array[i]);
    }

    return array;
}

static float* desigmoidize_inplace (float* array, const size_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        array[i] = inverse_sigmoid (array[i]);
    }

    return array;
}

float* calculate_output (const neuralnet_t* net, const float* input)
{
    assert (net != NULL);
    assert (input != NULL);

    size_t max = max_size (net->neurons_per_layer, net->layer_count);
    float* current_result_1 = SAFE_MALLOC (max * sizeof (float));
    float* current_result_2 = SAFE_MALLOC (max * sizeof (float));

    memcpy (current_result_1, input,
            net->neurons_per_layer[0] * sizeof (float));
    sigmoidize_inplace (current_result_1, net->neurons_per_layer[0]);

    for (uint32_t gap = 0; gap < net->layer_count - 1; ++gap)
    {
        for (uint32_t to = 0; to < net->neurons_per_layer[gap + 1]; ++to)
        {
            current_result_2[to] = 0.0f;

            for (uint32_t from = 0; from < net->neurons_per_layer[gap]; ++from)
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

// Backpropagation #####################################################

float learn_rate = 1.0f;

typedef struct{

	size_t layer_count;
	size_t* neurons_per_layer;	

	float** values;

} full_output_t;

static full_output_t* allocate_full_output(const neuralnet_t* net){
	
	full_output_t* full_output = SAFE_MALLOC(sizeof(full_output_t));
	
	full_output->layer_count = net->layer_count;
	full_output->neurons_per_layer = net->neurons_per_layer;
	
	full_output->values = SAFE_MALLOC(net->layer_count * sizeof(float*));
	for(uint32_t layer = 0; layer < net->layer_count; ++layer){

		full_output->values[layer] = SAFE_MALLOC(net->neurons_per_layer[layer] * sizeof(float));

	}

	return full_output;

}
static void free_full_output(full_output_t* full_output){
	
	for(uint32_t layer = 0; layer < full_output->layer_count; ++layer){
		free(full_output->values[layer]);
	}
	free(full_output->values);
	
	free(full_output);
	
}
static full_output_t* calculate_full_output(const neuralnet_t* net, const float* input){

	full_output_t* fout = allocate_full_output(net);

	float* ins = sigmoidize(input, net->neurons_per_layer[0]);

	memcpy(fout->values[0], ins, net->neurons_per_layer[0] * sizeof(float));

	for(uint32_t gap = 0; gap < net->layer_count - 1; ++gap){

		for(uint32_t to = 0; to < net->neurons_per_layer[gap + 1]; ++to){

			fout->values[gap + 1][to] = 0.0f;

			for(uint32_t from = 0; from < net->neurons_per_layer[gap]; ++from){

				fout->values[gap + 1][to] += fout->values[gap][from] * net->edges[gap][from][to];

			}

			fout->values[gap + 1][to] = sigmoid(fout->values[gap + 1][to]);
		
		}

	}

	free(ins);

	return fout;

}

static float calculate_sigmoid_error_term(float a){
	return a * (1.0f - a);
}

static float calculate_sigmoid_error(float target, float actual){
	return calculate_sigmoid_error_term(actual) * (target - actual);
}

void backpropagate(neuralnet_t* net, const float* input, const float* target_output){
    assert (net != NULL);
    assert (input != NULL);
    assert (target_output != NULL);

	full_output_t* fout = calculate_full_output(net, input);

	float* ins = sigmoidize(input, net->neurons_per_layer[0]);
	float* touts = sigmoidize(target_output, net->neurons_per_layer[net->layer_count - 1]);

	float** errors = SAFE_MALLOC(net->layer_count * sizeof(float*));

	// Calculating direct errors of last layer
	uint32_t last = net->layer_count - 1;
	
	errors[last] = SAFE_MALLOC(net->neurons_per_layer[last] * sizeof(float));

	for(uint32_t to = 0; to < net->neurons_per_layer[last]; ++to){

		errors[last][to] = calculate_sigmoid_error(touts[to], fout->values[last][to]);

		// Correcting edge weights of gap right before the last layer of neurons
		for(uint32_t from = 0; from < net->neurons_per_layer[last - 1]; ++from){

			net->edges[last - 1][from][to] += learn_rate * errors[last][to] * fout->values[last - 1][from];

		}

	}

	// Calculating intermediate errors of all remaining layers
	for(uint32_t layer = net->layer_count - 2; layer > 0; --layer){

		errors[layer] = SAFE_MALLOC(net->neurons_per_layer[layer] * sizeof(float));

		for(uint32_t current = 0; current < net->neurons_per_layer[layer]; ++current){

			errors[layer][current] = 0.0f;

			for(uint32_t right = 0; right < net->neurons_per_layer[layer + 1]; ++right){

				errors[layer][current] += learn_rate * errors[layer + 1][right] * net->edges[layer][current][right];

			}

			errors[layer][current] *= calculate_sigmoid_error_term(fout->values[layer][current]);

			// Correcting edge weights of gap right before the present layer of neurons
			for(uint32_t left = 0; left < net->neurons_per_layer[layer - 1]; ++left){

				net->edges[layer - 1][left][current] += errors[layer][current] * fout->values[layer - 1][left];

			}

		}

	}

	// Clean up
	free_full_output(fout);
	free(ins);
	free(touts);
	for(uint32_t layer = 1; layer < net->layer_count; ++layer)
		free(errors[layer]);
	free(errors);

}

void print_neuralnet(const neuralnet_t* net){
    assert (net != NULL);

	printf("\nNeural network at Adress x:");

	for(uint32_t gap = 0; gap < net->layer_count - 1; ++gap){

		printf("\n\tGap %d:", gap);

		for(uint32_t to = 0; to < net->neurons_per_layer[gap + 1]; ++to){

			printf("\n\t\tTo Neuron %d:", to);

			for(uint32_t from = 0; from < net->neurons_per_layer[gap]; ++from){

				printf(" %f", net->edges[gap][from][to]);

			}

		}

	}

	printf("\n");

}

static float zoo(){ // Zero or one
	return random_value_01() > 0.5f ? 1.0f : 0.0f;
}

/*
int main(int argc, char** argv){

	uint32_t nphl[] = {5, 5};
	neuralnet_t* net = create_neural_net_random_new(2, nphl);

	learn_rate = 10.0f;

	print_neuralnet(net);

	for(uint32_t i = 0; i < 10000; ++i){

		float ins[5] = { zoo(), zoo(), zoo(), zoo(), zoo() }; 	// Input of zeros and ones
		float* target = ins;					// This neural net should give the same binary sequence it got as input

		if(i%100 == 0){
			float* out = calculate_output_new(net, ins);
			printf("Error: %f %f %f %f %f\n", target[0] - out[0], target[1] - out[1], target[2] - out[2], target[3] - out[3], target[4] - out[4]);
			free(out);
		}
		backpropagate(net, ins, target);
	}

	printf("\n");
	print_neuralnet(net); // It works!

	destroy_neural_net_new(net);

	return 0;

}*/


void neural_net_to_file (neuralnet_t* net, const char* path, bool binary)
{
}
neuralnet_t* neural_net_from_file (const char* path, bool binary)
{
    return NULL;
}
