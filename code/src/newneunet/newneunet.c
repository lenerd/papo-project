#include "newneunet.h"
#include "../math_extend/math_ext.h"

#include <stdlib.h>
#include <stdio.h>

neuralnet_t* create_neural_net_random(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count){
	
	neuralnet_t* net = malloc(sizeof(neuralnet_t));

	net->input_count = input_count;
	net->hidden_layer_count = hidden_layer_count;
	net->neurons_per_hidden_layer = neurons_per_hidden_layer;
	net->output_count = output_count;

	//Input to hidden layers, edge weights
	net->input_weights = malloc(input_count * sizeof(float*));
	for(uint32_t i = 0; i < input_count; ++i){
		net->input_weights[i] = malloc(neurons_per_hidden_layer * sizeof(float));
		for(uint32_t j = 0; j < neurons_per_hidden_layer; ++j){
			net->input_weights[i][j] = inverse_sigmoid(random_value_01());
		}
	}

	//Hidden layer intern
	net->hidden_layer
} 

int main(int argc, char** argv){

	return 0;

}
