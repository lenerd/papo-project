#include "newneunet.h"
#include "util/util.h"
#include "util/math_ext.h"

#include <assert.h>
#include <string.h>

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
