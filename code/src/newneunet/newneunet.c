#include "newneunet.h"
#include "../util/util.h"
#include "../util/math_ext.h"

#include <stdlib.h>
#include <stdio.h>

static void initialize_input_weights_random(neuralnet_t* net){
	
	net->input_weights = SAFE_MALLOC(float**, 1);
	*net->input_weights = SAFE_MALLOC(float*, net->neurons_per_hidden_layer);
	for(uint32_t to = 0; to < neurons_per_hidden_layer; ++to){
		*net->input_weights[to] = SAFE_MALLOC(float, net->input_count + 1); // +1 for bias
		for(uint32_t from = 0; from < net->input_count + 1; ++from){
			*net->input_weights[to][from] = random_value_mm(-1.0f, 1.0f);
		}
	}
	
}
static void initialize_hidden_weights_random(neuralnet_t* net){
	
	net->hidden_layer = SAFE_MALLOC(float***, 1);
	*net->hidden_layer = SAFE_MALLOC(float**, net->hidden_layer_count - 1);
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		*net->hidden_layer[gap] = SAFE_MALLOC(float*, net->neurons_per_hidden_layer);
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			*net->hidden_layer[gap][to] = SAFE_MALLOC(float, net->neurons_per_hidden_layer + 1); // +1 for bias
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer + 1; ++from){
				*net->hidden_layer_weights[gap][to][from] = random_value_mm(-1.0f, 1.0f);
			}
		}
	}
	
}
static void initialize_output_weights_random(neuralnet_t* net){
	
	net->output_weights = SAFE_MALLOC(float**, 1);
	*net->output_weights = SAFE_MALLOC(float*, net->output_count);
	for(uint32_t to = 0; to < net->output_count; ++to){
		*net->output_weights[to] = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
		for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
			*net->input_weights[to][from] = random_value_mm(-1.0f, 1.0f);
		}
	}
	
}

static void destroy_input_weights(neuralnet_t* net){
	
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		free(*net->input_weights[to]);
	}
	free(*net->input_weights);
	free(net->input_weights);
	
}
static void destroy_hidden_weights(neuralnet_t* net){
	
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			free(*net->hidden_layer[gap][to]);
		}
		free(*net->hidden_layer[gap]);
	}
	free(*net->hidden_layer);
	free(net->hidden_layer);
	
}
static void destroy_output_weights(neuralnet_t* net){
	
	for(uint32_t to = 0; to < net->output_count; ++to){
		free(*net->output_weights[to]);
	}
	free(*net->output_weights);
	free(net->output_weights);
	
}

neuralnet_t* create_neural_net_random(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count){
	
	neuralnet_t* net = SAFE_MALLOC(neuralnet_t, 1);

	net->input_count = input_count;
	net->hidden_layer_count = hidden_layer_count;
	net->neurons_per_hidden_layer = neurons_per_hidden_layer;
	net->output_count = output_count;

	initialize_input_weights_random(net);
	initialize_hidden_weights_random(net);
	initialize_output_weights_random(net);
	
	return net;
} 

void destroy_neural_net(neuralnet_t* net){
	
	destroy_output_weights(net);
	destroy_hidden_weights(net);
	destroy_input_weights(net);
	
	free(net);
	
}

float** calculate_output(const neuralnet_t* net, const float* input){
	
	float* current_result_1 = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
	float* current_result_2 = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
	
	// Check sigmoid function!!
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		for(uint32_t from = 0; from < net->input_count; ++from){
			current_result_1[to] += input[from] * (*net->input_weights)[to][from];
		}
		current_result_2[to] = sigmoid(current_result_1);
	}
	
	// Check sigmoid function!!
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
				current_result_2[to] += current_result_1[from] * (*net->hidden_layer_weights)[gap][to][from]; 
			}
		}
		swap_buffer(current_result_1, current_result_2);
	}
	
}

int main(int argc, char** argv){

	return 0;

}
