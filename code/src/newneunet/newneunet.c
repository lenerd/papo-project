#include "newneunet.h"
#include "../util/util.h"
#include "../util/math_ext.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <omp.h>

// Construction and Destruction ########################################

static void initialize_weights_random(neuralnet_t* net){

	net->weights = malloc((net->layer_count - 1) * sizeof(float**));
	CHECK_MALLOC(net->weights)

	#pragma omp parallel for
	for(uint32_t gap = 0; gap < net->layer_count - 1; ++gap){
		
		net->weights[gap] = malloc(net->neurons_per_layer_count[gap + 1] * sizeof(float*));
		CHECK_MALLOC(net->weights[gap]);
		
		#pragma omp parallel for
		for(uint32_t to = 0; to < net->neurons_per_layer_count[gap + 1]; ++to){

			net->weights[gap][to] = malloc(net->neurons_per_layer_count[gap] * sizeof(float));
			CHECK_MALLOC(net->weights[gap][to])
			
			#pragma omp parallel for
			for(uint32_t from = 0; from < net->neurons_per_layer_count[gap]; ++from){

				net->weights[gap][to][from] = random_value_mm(-1.0f, 1.0f);

			}

		}

	}

}
static void destroy_weights(neuralnet_t* net){

	#pragma omp parallel for
	for(uint32_t gap = 0; gap < net->layer_count - 1; ++gap){
		
		#pragma omp parallel for
		for(uint32_t to = 0; to < net->neurons_per_layer_count[gap + 1]; ++to){

			free(net->weights[gap][to]);

		}

		free(net->weights[gap]);

	}

	free(net->weights);

}

neuralnet_t* create_neural_net_random_new(const uint32_t layer_count, uint32_t* neurons_per_layer_count){
	
	neuralnet_t* net = malloc(sizeof(neuralnet_t));
	CHECK_MALLOC(net);

	net->layer_count = layer_count;
	net->neurons_per_layer_count = neurons_per_layer_count;
	
	initialize_weights_random(net);
	
	return net;
} 

void destroy_neural_net_new(neuralnet_t* net){
	
	destroy_weights(net);
	
	free(net);
	
}

// Calculation #########################################################
static float* sigmoidize(const float* array, const uint32_t size){
	
	float* result = malloc(size * sizeof(float));
	CHECK_MALLOC(result);

	#pragma omp parallel for	
	for(uint32_t i = 0; i < size; ++i){
		result[i] = sigmoid(array[i]);
	}

	return result;
}

static float* desigmoidize(const float* array, const uint32_t size){
	
	float* result = malloc(size * sizeof(float));
	CHECK_MALLOC(result);
	
	#pragma omp parallel for
	for(uint32_t i = 0; i < size; ++i){
		result[i] = inverse_sigmoid(array[i]);
	}

	return result;
}

float* calculate_output_new(const neuralnet_t* net, const float* input){

 	float* current_result_1 = sigmoidize(input, net->neurons_per_layer_count[0]);

	for(uint32_t gap = 0; gap < net->layer_count - 1; ++gap){

		float* current_result_2 = malloc(net->neurons_per_layer_count[gap + 1] * sizeof(float));
		CHECK_MALLOC(current_result_2)

		#pragma omp parallel for
		for(uint32_t to = 0; to < net->neurons_per_layer_count[gap + 1]; ++to){

			current_result_2[to] = 0.0f;

			for(uint32_t from = 0; from < net->neurons_per_layer_count[gap]; ++from){

				current_result_2[to] += current_result_1[from] * net->weights[gap][to][from];

			}

			current_result_2[to] = sigmoid(current_result_2[to]);

		}

		swap_float_buffer(&current_result_1, &current_result_2);
		free(current_result_2);

	}

	return desigmoidize(current_result_1, net->neurons_per_layer_count[net->layer_count - 1]);

}

// Backpropagation #####################################################

float learn_rate = 1.0f;

typedef struct{

	uint32_t layer_count;
	uint32_t* neurons_per_layer_count;	

	float** values;

} full_output_t;

static full_output_t* allocate_full_output(const neuralnet_t* net){
	
	full_output_t* full_output = malloc(sizeof(full_output_t));
	CHECK_MALLOC(full_output);
	
	full_output->layer_count = net->layer_count;
	full_output->neurons_per_layer_count = net->neurons_per_layer_count;
	
	full_output->values = malloc(net->layer_count * sizeof(float*));
	CHECK_MALLOC(full_output->values)

	#pragma omp parallel for
	for(uint32_t layer = 0; layer < net->layer_count; ++layer){

		full_output->values[layer] = malloc(net->neurons_per_layer_count[layer] * sizeof(float));
		CHECK_MALLOC(full_output->values[layer])

	}

	return full_output;

}
static void free_full_output(full_output_t* full_output){
	
	#pragma omp parallel for
	for(uint32_t layer = 0; layer < full_output->layer_count; ++layer){
		free(full_output->values[layer]);
	}
	free(full_output->values);
	
	free(full_output);
	
}
static full_output_t* calculate_full_output(const neuralnet_t* net, const float* input){

	full_output_t* fout = allocate_full_output(net);

	float* ins = sigmoidize(input, net->neurons_per_layer_count[0]);

	memcpy(fout->values[0], ins, net->neurons_per_layer_count[0] * sizeof(float));

	for(uint32_t gap = 0; gap < net->layer_count - 1; ++gap){
		
		#pragma omp parallel for
		for(uint32_t to = 0; to < net->neurons_per_layer_count[gap + 1]; ++to){

			fout->values[gap + 1][to] = 0.0f;

			for(uint32_t from = 0; from < net->neurons_per_layer_count[gap]; ++from){

				fout->values[gap + 1][to] += fout->values[gap][from] * net->weights[gap][to][from];

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

	full_output_t* fout = calculate_full_output(net, input);

	float* ins = sigmoidize(input, net->neurons_per_layer_count[0]);
	float* touts = sigmoidize(target_output, net->neurons_per_layer_count[net->layer_count - 1]);

	float** errors = malloc(net->layer_count * sizeof(float*));
	CHECK_MALLOC(errors)

	// Calculating direct errors of last layer
	uint32_t last = net->layer_count - 1;
	
	errors[last] = malloc(net->neurons_per_layer_count[last] * sizeof(float));
	CHECK_MALLOC(errors[last]);

	#pragma omp parallel for
	for(uint32_t to = 0; to < net->neurons_per_layer_count[last]; ++to){

		errors[last][to] = calculate_sigmoid_error(touts[to], fout->values[last][to]);

		// Correcting edge weights of gap right before the last layer of neurons
		for(uint32_t from = 0; from < net->neurons_per_layer_count[last - 1]; ++from){

			net->weights[last - 1][to][from] += learn_rate * errors[last][to] * fout->values[last - 1][from];

		}

	}

	// Calculating intermediate errors of all remaining layers
	for(uint32_t layer = net->layer_count - 2; layer > 0; --layer){

		errors[layer] = malloc(net->neurons_per_layer_count[layer] * sizeof(float));
		CHECK_MALLOC(errors[layer]);

		#pragma omp parallel for
		for(uint32_t current = 0; current < net->neurons_per_layer_count[layer]; ++current){

			errors[layer][current] = 0.0f;

			for(uint32_t right = 0; right < net->neurons_per_layer_count[layer + 1]; ++right){

				errors[layer][current] += learn_rate * errors[layer + 1][right] * net->weights[layer][right][current];

			}

			errors[layer][current] *= calculate_sigmoid_error_term(fout->values[layer][current]);

			// Correcting edge weights of gap right before the present layer of neurons
			for(uint32_t left = 0; left < net->neurons_per_layer_count[layer - 1]; ++left){

				net->weights[layer - 1][current][left] += errors[layer][current] * fout->values[layer - 1][left];

			}

		}

	}

	// Clean up
	free_full_output(fout);
	free(ins);
	free(touts);
	#pragma omp parallel for
	for(uint32_t layer = 1; layer < net->layer_count; ++layer)
		free(errors[layer]);
	free(errors);

}

void print_neuralnet(const neuralnet_t* net){

	printf("\nNeural network at Adress x:");

	for(uint32_t gap = 0; gap < net->layer_count - 1; ++gap){

		printf("\n\tGap %d:", gap);

		for(uint32_t to = 0; to < net->neurons_per_layer_count[gap + 1]; ++to){

			printf("\n\t\tTo Neuron %d:", to);

			for(uint32_t from = 0; from < net->neurons_per_layer_count[gap]; ++from){

				printf(" %f", net->weights[gap][to][from]);

			}

		}

	}

	printf("\n");

}

static float zoo(){ // Zero or one
	return random_value_01() > 0.5f ? 1.0f : 0.0f;
}

int main(int argc, char** argv){

	uint32_t h_lays = 2;
	uint32_t nphl[] = {100, 100};

	learn_rate = 10.0f;

	uint32_t iters = 10000;

	printf("Testing performance for a neuralnet with following neuron per hidden layer counts:\n\t");
	for(uint32_t i = 0; i < h_lays; ++i){

		printf("%d, ", nphl[i]);

	}

	neuralnet_t* net = create_neural_net_random_new(h_lays, nphl);

	printf("\n\nStarting test...\n");
	
	clock_t start, stop;
	start = clock();

	float* ins = malloc(100 * sizeof(float));
	CHECK_MALLOC(ins)

	float* target = ins;

	for(uint32_t i = 0; i < iters; ++i){

		/*if(i%100 == 0){
			float* out = calculate_output_new(net, ins);
			printf("Error: %f %f %f %f %f\n", target[0] - out[0], target[1] - out[1], target[2] - out[2], target[3] - out[3], target[4] - out[4]);
			free(out);
		}*/
		backpropagate(net, ins, target);
	}

	//printf("\n");
	//print_neuralnet(net); // It works!

	stop = clock();

	destroy_neural_net_new(net);

	printf("Time in milliseconds for %d backpropagations: %d\n", iters, 1000 * (stop - start) / CLOCKS_PER_SEC); 

	return 0;

}
