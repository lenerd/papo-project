#ifndef NEWNEUNET_H
#define NEWNEUNET_H

#include <stdint.h>

typedef struct{

	uint32_t input_count;
	uint32_t hidden_layer_count;
	uint32_t neurons_per_hidden_layer;	
	uint32_t output_count;

	float* input_weights[][];
	float* hidden_layer_weights[][][];
	float* output_weights[][];

} neuralnet_t;

neuralnet_t* create_neural_net_random(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count);

float** calculate_output(neuralnet_t* net);

#endif
