#include "newneunet.h"
#include "../math_extend/math_ext.h"

#include <stdlib.h>
#include <stdio.h>

static void initialize_input_weights_random(neuralnet_t* net){
	
	net->input_weights = malloc(sizeof float**)
	*net->input_weights = malloc((input_count + 1) * sizeof(float*)); //+1 for bias
	for(uint32_t from = 0; from < input_count + 1; ++from){
		*net->input_weights[from] = malloc(neurons_per_hidden_layer * sizeof(float));
		for(uint32_t to = 0; to < neurons_per_hidden_layer; ++to){
			*net->input_weights[from][to] = random_value_mm(-1.0f, 1.0f);
		}
	}
	
}
static void initialize_hidden_weights_random(neuralnet_t* net){
	
	net->hidden_layer = malloc(sizeof(float***));
	*net->hidden_layer = malloc((hidden_layer_count - 1) * sizeof(float**));
	for(uint32_t gap = 0; gap < hidden_layer_count - 1; ++gap){
		*net->hidden_layer[gap] = malloc((neurons_per_hidden_layer + 1) * sizeof(float*)); //+1 for bias
		for(uint32_t from = 0; from < neurons_per_hidden_layer + 1; ++from){
			*net->hidden_layer[gap][from] = malloc((neurons_per_hidden_layer) * sizeof(float));
			for(uint32_t to = 0; to < neurons_per_hidden_layer; ++to){
				*net->hidden_layer_weights[gap][from][to] = random_value_mm(-1.0f, 1.0f);
			}
		}
	}
	
}
static void initialize_output_weights_random(neuralnet_t* net){
	
	net->output_weights = malloc(sizeof float**)
	*net->output_weights = malloc(output_weights * sizeof(float*));
	for(uint32_t from = 0; from < neurons_per_hidden_layer; ++from){
		*net->output_weights[from] = malloc(neurons_per_hidden_layer * sizeof(float));
		for(uint32_t to = 0; to < output_count; ++to){
			*net->input_weights[from][to] = random_value_mm(-1.0f, 1.0f);
		}
	}
	
}

static void destroy_input_weights(neuralnet_t* net){
	
	for(uint32_t from = 0; from < input_count + 1; ++from){
		free(*net->input_weights[from]);
	}
	free(*net->input_weights);
	free(net->input_weights);
	
}
static void destroy_hidden_weights(neuralnet_t* net){
	
	for(uint32_t gap = 0; gap < hidden_layer_count - 1; ++gap){
		for(uint32_t from = 0; from < neurons_per_hidden_layer + 1; ++from){
			free(*net->hidden_layer[gap][from]);
		}
		free(*net->hidden_layer[gap]);
	}
	free(*net->hidden_layer);
	free(net->hidden_layer);
	
}
static void destroy_output_weights(neuralnet_t* net){
	
	for(uint32_t from = 0; from < neurons_per_hidden_layer; ++from){
		free(*net->output_weights[from]);
	}
	free(*net->output_weights);
	free(net->output_weights);
	
}

neuralnet_t* create_neural_net_random(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count){
	
	neuralnet_t* net = malloc(sizeof(neuralnet_t));

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



int main(int argc, char** argv){

	return 0;

}
