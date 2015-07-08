#include "newneunet.h"
#include "../util/util.h"
#include "../util/math_ext.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Construction and Destruction ########################################

static void initialize_input_weights_random(neuralnet_t* net){
	
	net->input_weights = malloc(net->neurons_per_hidden_layer * sizeof(float*));
	CHECK_MALLOC(net->input_weights)
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		net->input_weights[to] = malloc(net->input_count * sizeof(float));
		CHECK_MALLOC(net->input_weights[to])
		for(uint32_t from = 0; from < net->input_count; ++from){
			net->input_weights[to][from] = random_value_mm(-1.0f, 1.0f);
		}
	}
	
}
static void initialize_hidden_weights_random(neuralnet_t* net){
	
	net->hidden_weights = malloc((net->hidden_layer_count - 1) * sizeof(float**));
	CHECK_MALLOC(net->hidden_weights)
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		net->hidden_weights[gap] = malloc(net->neurons_per_hidden_layer * sizeof(float*));
		CHECK_MALLOC(net->hidden_weights[gap])
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			net->hidden_weights[gap][to] = malloc(net->neurons_per_hidden_layer * sizeof(float));
			CHECK_MALLOC(net->hidden_weights)
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
				net->hidden_weights[gap][to][from] = random_value_mm(-1.0f, 1.0f);
			}
		}
	}
	
}
static void initialize_output_weights_random(neuralnet_t* net){
	
	net->output_weights = malloc(net->output_count * sizeof(float*));
	CHECK_MALLOC(net->output_weights)
	for(uint32_t to = 0; to < net->output_count; ++to){
		net->output_weights[to] = malloc(net->neurons_per_hidden_layer * sizeof(float));
		CHECK_MALLOC(net->output_weights[to])
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

neuralnet_t* create_neural_net_random_new(const uint32_t input_count, const uint32_t hidden_layer_count, const uint32_t neurons_per_hidden_layer, const uint32_t output_count){
	
	neuralnet_t* net = malloc(sizeof(neuralnet_t));
	CHECK_MALLOC(net);

	net->input_count = input_count;
	net->hidden_layer_count = hidden_layer_count;
	net->neurons_per_hidden_layer = neurons_per_hidden_layer;
	net->output_count = output_count;

	initialize_input_weights_random(net);
	initialize_hidden_weights_random(net);
	initialize_output_weights_random(net);
	
	return net;
} 

void destroy_neural_net_new(neuralnet_t* net){
	
	destroy_output_weights(net);
	destroy_hidden_weights(net);
	destroy_input_weights(net);
	
	free(net);
	
}

// Calculation #########################################################

float* calculate_output_new(const neuralnet_t* net, const float* input){
	
	float* current_result_1 = malloc(net->neurons_per_hidden_layer * sizeof(float));
	CHECK_MALLOC(current_result_1)
	float* current_result_2 = malloc(net->neurons_per_hidden_layer * sizeof(float));
	CHECK_MALLOC(current_result_2)
	
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		current_result_2[to] = 0.0f;
		for(uint32_t from = 0; from < net->input_count; ++from){
			current_result_2[to] += input[from] * net->input_weights[to][from];
		}
		current_result_2[to] = sigmoid(current_result_2[to]);
	}
	swap_float_buffer(&current_result_1, &current_result_2);
	
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			current_result_2[to] = 0.0f;
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
				current_result_2[to] += current_result_1[from] * net->hidden_weights[gap][to][from]; 
			}
			current_result_2[to] = sigmoid(current_result_2[to]);
			
		}
		swap_float_buffer(&current_result_1, &current_result_2);
	}
	
	for(uint32_t to = 0; to < net->output_count; ++to){
		current_result_2[to] = 0.0f;
		for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
			current_result_2[to] += current_result_1[from] * net->output_weights[to][from];
		}
	}
	swap_float_buffer(&current_result_1, &current_result_2);
	
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
	
	full_output_t* full_output = malloc(sizeof(full_output_t));
	CHECK_MALLOC(full_output);
	
	full_output->input_count = net->input_count;
	full_output->hidden_layer_count = net->hidden_layer_count;
	full_output->neurons_per_hidden_layer = net->neurons_per_hidden_layer;
	full_output->output_count = net->output_count;
	
	full_output->input_values = malloc(net->input_count * sizeof(float));
	CHECK_MALLOC(full_output->input_values)
	
	full_output->hidden_values = malloc(net->hidden_layer_count * sizeof(float*));
	CHECK_MALLOC(full_output->hidden_values)
	for(uint32_t hl = 0; hl < net->hidden_layer_count; ++hl){
		full_output->hidden_values[hl] = malloc(net->neurons_per_hidden_layer * sizeof(float));
		CHECK_MALLOC(full_output->hidden_values[hl])
	}
	
	full_output->output_values = malloc(net->output_count * sizeof(float));
	CHECK_MALLOC(full_output->output_values)
	
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
	
	memcpy(full_output->input_values, input, net->input_count * sizeof(float));

	float* current_result_1 = malloc(net->neurons_per_hidden_layer * sizeof(float));
	CHECK_MALLOC(current_result_1)
	float* current_result_2 = malloc(net->neurons_per_hidden_layer * sizeof(float));
	CHECK_MALLOC(current_result_2)
	
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		current_result_2[to] = 0.0f;
		for(uint32_t from = 0; from < net->input_count; ++from){
			current_result_2[to] += input[from] * net->input_weights[to][from];
		}
		full_output->hidden_values[0][to] = sigmoid(current_result_2[to]);
	}
	swap_float_buffer(&current_result_1, &current_result_2);
	
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			current_result_2[to] = 0.0f;
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
				current_result_2[to] += current_result_1[from] * net->hidden_weights[gap][to][from]; 
			}
			full_output->hidden_values[gap + 1][to] = sigmoid(current_result_2[to]);
		}
		swap_float_buffer(&current_result_1, &current_result_2);
	}
	
	for(uint32_t to = 0; to < net->output_count; ++to){
		current_result_2[to] = 0.0f;
		for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
			current_result_2[to] += current_result_1[from] * net->output_weights[to][from];
		}
		full_output->output_values[to] = sigmoid(current_result_2[to]);
	}
	
	free(current_result_1);
	free(current_result_2);
	
	return full_output;
	
}

void backpropagate(neuralnet_t* net, const float* input, const float* target_output){
	
	full_output_t* full_output = calculate_full_output(net, input);
	
	// Error calculation
	
	// Output layer error calculation
	float* output_errors = malloc(full_output->output_count * sizeof(float));
	CHECK_MALLOC(output_errors);
	for(uint32_t to = 0; to < net->output_count; ++to){
		output_errors[to] = full_output->output_values[to] * (1 - full_output->output_values[to]) * (sigmoid(target_output[to]) - full_output->output_values[to]);
	}
	
	// Hidden layer error calculation
	float** hidden_errors = malloc(full_output->hidden_layer_count * sizeof(float*));
	CHECK_MALLOC(hidden_errors)
	
	hidden_errors[full_output->hidden_layer_count - 1] = malloc(full_output->neurons_per_hidden_layer * sizeof(float));
	CHECK_MALLOC(hidden_errors[full_output->hidden_layer_count - 1])
	for(uint32_t from = 0; from < full_output->neurons_per_hidden_layer; ++from){
		hidden_errors[full_output->hidden_layer_count - 1][from] = 0.0f;
		for(uint32_t to = 0; to < full_output->output_count; ++to){
			hidden_errors[full_output->hidden_layer_count - 1][from] += output_errors[to] * net->output_weights[to][from];
		}
		hidden_errors[full_output->hidden_layer_count - 1][from] *= full_output->hidden_values[full_output->hidden_layer_count - 1][from] * (1.0f - full_output->hidden_values[full_output->hidden_layer_count - 1][from]);
	}
	
	for(int layer = full_output->hidden_layer_count - 2; layer > -1; --layer){
		hidden_errors[layer] = malloc(full_output->neurons_per_hidden_layer * sizeof(float));
		CHECK_MALLOC(hidden_errors[layer])
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

void print_neuralnet(const neuralnet_t* net){

	printf("Input to hidden layer edge weights:");
	for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
		printf("\n\tTo neuron %d: ", to);
		for(uint32_t from = 0; from < net->input_count; ++from){
			printf("%f ", net->input_weights[to][from]);
		}
	}

	printf("\n\nHidden layer internal edge weights:");
	for(uint32_t gap = 0; gap < net->hidden_layer_count - 1; ++gap){
		printf("\n\tGap %d:", gap);
		for(uint32_t to = 0; to < net->neurons_per_hidden_layer; ++to){
			printf("\n\t\tTo neuron %d: ", to);
			for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
				printf("%f ", net->hidden_weights[gap][to][from]);
			}
		}
	}

	printf("\n\nHidden to output layer edge weights:");
	for(uint32_t to = 0; to < net->output_count; ++to){
		printf("\n\tTo neuron %d: ", to);
		for(uint32_t from = 0; from < net->neurons_per_hidden_layer; ++from){
			printf("%f ", net->output_weights[to][from]);
		}
	}
	printf("\n\n");
}

int main(int argc, char** argv){

	neuralnet_t* net = create_neural_net_random_new(2, 1, 1, 1);
	
	for(uint32_t i = 0; i < 500; ++i){

		float ins[2] = { random_value_mm(-100.0f, 100.0f), random_value_mm(-100.0f, 100.0f) };
		float target[1] = { ins[0] + ins[1] };

		backpropagate(net, ins, target);
		if(i%10 == 0){
			float* out = calculate_output_new(net, ins);
			printf("Target: %f, Output: %f\n", target[0], out[0]);
			free(out);
		}
	}

	print_neuralnet(net);

	destroy_neural_net_new(net);

	/*
	uint32_t input_count = 2;
	uint32_t hidden_layer_count = 1;
	uint32_t neurons_per_hidden_layer = 1;
	uint32_t output_count = 1;

	uint32_t trainings = 1000;

	neuralnet_t* net = create_neural_net_random_new(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);

	for(uint32_t i = 0; i < trainings; ++i){
		
		float ins[] = { random_value_mm(-100.0f, 100.0f), random_value_mm(-100.0f, 100.0f) };

		float target[] = { ins[0] + ins[1] };

		float* output = calculate_output_new(net, ins);
		printf("Error: %f\n", target[0] - output[0]);

		backpropagate(net, ins, target);

	}

	destroy_neural_net_new(net);
	*/
	return 0;

}
