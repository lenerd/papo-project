#include "newneunet.h"
#include "../util/util.h"
#include "../util/math_ext.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Construction and Destruction ########################################

static void initialize_input_weights_random(neuralnet_t* net){
	
	net->input_weights = SAFE_MALLOC(float*, net->neurons_per_hidden_layer);
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		net->input_weights[to] = SAFE_MALLOC(float, net->input_count);
		for(uint32_t from = 0; from < net->input_count; ++from){
			net->input_weights[to][from] = random_value_mm(-1.0f, 1.0f);
		}
	}
	
}
static void initialize_hidden_weights_random(neuralnet_t* net){
	
	net->hidden_weights = SAFE_MALLOC(float**, net->hidden_layer_count - 1);
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		net->hidden_weights[gap] = SAFE_MALLOC(float*, net->neurons_per_hidden_layer);
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			net->hidden_weights[gap][to] = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
				net->hidden_weights[gap][to][from] = random_value_mm(-1.0f, 1.0f);
			}
		}
	}
	
}
static void initialize_output_weights_random(neuralnet_t* net){
	
	net->output_weights = SAFE_MALLOC(float*, net->output_count);
	for(uint32_t to = 0; to < net->output_count; ++to){
		net->output_weights[to] = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
		for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
			net->output_weights[to][from] = random_value_mm(-1.0f, 1.0f);
		}
	}
	
}

static void destroy_input_weights(neuralnet_t* net){
	
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		free(net->input_weights[to]);
	}
	free(net->input_weights);
	
}
static void destroy_hidden_weights(neuralnet_t* net){
	
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			free(net->hidden_weights[gap][to]);
		}
		free(net->hidden_weights[gap]);
	}
	free(net->hidden_weights);
	
}
static void destroy_output_weights(neuralnet_t* net){
	
	for(uint32_t to = 0; to < net->output_count; ++to){
		free(net->output_weights[to]);
	}
	free(net->output_weights);
	
}

neuralnet_t* create_neural_net_random(const uint32_t input_count, const uint32_t hidden_layer_count, const uint32_t neurons_per_hidden_layer, const uint32_t output_count){
	
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

// Calculation #########################################################

float* calculate_output(const neuralnet_t* net, const float* input){
	
	float* current_result_1 = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
	float* current_result_2 = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
	
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		current_result_2[to] = 0.0f;
		for(uint32_t from = 0; from < net->input_count; ++from){
			current_result_2[to] += input[from] * net->input_weights[to][from];
		}
		current_result_2[to] = sigmoid(current_result_2[to]);
	}
	swap_buffer(current_result_1, current_result_2);
	
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			current_result_2[to] = 0.0f;
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
				current_result_2[to] += current_result_1[from] * net->hidden_weights[gap][to][from]; 
			}
			current_result_2[to] = sigmoid(current_result_2[to]);
			
		}
		swap_buffer(current_result_1, current_result_2);
	}
	
	for(uint32_t to = 0; to < net->output_count; ++to){
		current_result_2[to] = 0.0f;
		for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
			current_result_2[to] += current_result_1[from] * net->output_weights[to][from];
		}
		current_result_2[to] = sigmoid(current_result_2[to]);
	}
	swap_buffer(current_result_1, current_result_2);
	
	free(current_result_1);
	free(current_result_2);
	
	return current_result_1;
	
}

// Backpropagation #####################################################

typedef struct{
	
	uint32_t input_count;
	uint32_t hidden_layer_count;
	uint32_t neurons_per_hidden_layer;
	uint32_t output_count;
	
	float* input_values;
	float** hidden_values;
	float* output_values;
	
} full_output_t;
static full_output_t* allocate_full_output(const neuralnet_t* net){
	
	full_output_t* full_output = SAFE_MALLOC(full_output_t, 1);
	
	full_output->input_count = net->input_count;
	full_output->hidden_layer_count = net->hidden_layer_count;
	full_output->neurons_per_hidden_layer = net->neurons_per_hidden_layer;
	full_output->output_count = net->output_count;
	
	full_output->input_values = SAFE_MALLOC(float, net->input_count);
	
	full_output->hidden_values = SAFE_MALLOC(float*, net->hidden_layer_count);
	for(uint32_t hl = 0; hl < net->hidden_layer_count; ++hl){
		full_output->hidden_values[hl] = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
	}
	
	full_output->output_values = SAFE_MALLOC(float, net->output_count);
	
	return full_output;

}
static void free_full_output(full_output_t* full_output){
	
	free(full_output->output_values);
	
	for(uint32_t hl = 0; hl < full_output->hidden_layer_count; ++hl){
		free(full_output->hidden_values[hl]);
	}
	free(full_output->hidden_values);
	
	free(full_output->input_values);
	
	free(full_output);
	
}
static full_output_t* calculate_full_output(const neuralnet_t* net, const float* input){
	
	full_output_t* full_output = allocate_full_output(net);
	
	memcpy(full_output->input_values, input, net->input_count);
	
	float* current_result_1 = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
	float* current_result_2 = SAFE_MALLOC(float, net->neurons_per_hidden_layer);
	
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		current_result_2[to] = 0.0f;
		for(uint32_t from = 0; from < net->input_count; ++from){
			current_result_2[to] += input[from] * net->input_weights[to][from];
		}
		full_output->hidden_values[0][to] = sigmoid(current_result_2[to]);
	}
	swap_buffer(current_result_1, current_result_2);
	
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			current_result_2[to] = 0.0f;
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
				current_result_2[to] += current_result_1[from] * net->hidden_weights[gap][to][from]; 
			}
			full_output->hidden_values[gap + 1][to] = sigmoid(current_result_2[to]);
		}
		swap_buffer(current_result_1, current_result_2);
	}
	
	for(uint32_t to = 0; to < net->output_count; ++to){
		current_result_2[to] = 0.0f;
		for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
			current_result_2[to] += current_result_1[from] * net->output_weights[to][from];
		}
		full_output->output_values[to] = sigmoid(current_result_2[to]);
	}
	swap_buffer(current_result_1, current_result_2);
	
	free(current_result_1);
	free(current_result_2);
	
	free_full_output(full_output);
	
	return full_output;
	
}

void backpropagate(neuralnet_t* net, const float* input, const float* target_output){
	
	full_output_t* full_output = allocate_full_output(net);
	full_output = calculate_full_output(net, input);
	
	// Error calculation
	
	// Output layer error calculation
	float* output_errors = SAFE_MALLOC(float, full_output->output_count);
	for(uint32_t to = 0; to < net->output_count; ++to){
		output_errors[to] = full_output->output_values[to] * (1 - full_output->output_values[to]) * (target_output[to] - full_output->output_values[to]);
	}
	
	// Hidden layer error calculation
	float** hidden_errors = SAFE_MALLOC(float*, full_output->hidden_layer_count);
	
	hidden_errors[full_output->hidden_layer_count - 1] = SAFE_MALLOC(float, full_output->neurons_per_hidden_layer);
	for(uint32_t from = 0; from < full_output->neurons_per_hidden_layer; ++from){
		hidden_errors[full_output->hidden_layer_count - 1][from] = 0.0f;
		for(uint32_t to = 0; to < full_output->output_count; ++to){
			hidden_errors[full_output->hidden_layer_count - 1][from] += output_errors[to] * net->output_weights[to][from];
		}
		hidden_errors[full_output->hidden_layer_count - 1][from] *= full_output->hidden_values[full_output->hidden_layer_count - 1][from] * (1.0f - full_output->hidden_values[full_output->hidden_layer_count - 1][from]);
	}
	
	for(int layer = full_output->hidden_layer_count - 2; layer > -1; --layer){
		hidden_errors[layer] = SAFE_MALLOC(float, full_output->neurons_per_hidden_layer);
		for(uint32_t from = 0; from < full_output->neurons_per_hidden_layer; ++from){
			hidden_errors[layer][from] = 0.0f;
			for(uint32_t to = 0; to < full_output->output_count; ++to){
				hidden_errors[layer][from] += hidden_errors[layer + 1][to] * net->hidden_weights[layer][to][from];
			}
			hidden_errors[layer][from] *= full_output->hidden_values[layer][from] * (1.0f - full_output->hidden_values[layer][from]);
		}
	}
	
	// Edge weight correction
	
	// Hidden to output layer edge weights
	for(uint32_t to = 0; to < net->output_count; ++to){
		for(uint32_t from = 0; from < net->output_count; ++from){
			net->output_weights[to][from] += output_errors[to] * full_output->hidden_values[full_output->hidden_layer_count - 1][from];
		}
	}
	
	// Hidden layer internal edge weights
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		for(uint32_t to = 0; to < net->output_count; ++to){
			for(uint32_t from = 0; from < net->output_count; ++from){
				net->hidden_weights[gap][to][from] += hidden_errors[gap + 1][to] * full_output->hidden_values[gap][from];
			}
		}
	}
	
	// Input to hidden layer edge weights
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		for(uint32_t from = 0; from < net->input_count; ++from){
			net->input_weights[to][from] += hidden_errors[0][to] * full_output->input_values[from];
		}
	}
	
	free_full_output(full_output);
	free(output_errors);
	for(uint32_t i = 0; i < net->hidden_layer_count; ++i){
		free(hidden_errors[i]);
	}
	free(hidden_errors);
	
}

int main(int argc, char** argv){
	
	test();
	return 0;

}
