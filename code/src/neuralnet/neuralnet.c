#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "neuralnet.h"

//Math and Random
#define E 2.7182818284

float sigmoid(float x)		   { return 1.0f / (1.0f + (float)pow(E, -x)); } //Mathematical sigmoid function for the intensity of firing neurons
float inverse_sigmoid(float x) { return (float)(-log(1.0f / x - 1.0f)); }	 //Inverse of the sigmoid function for initial edge weights of the neuralnet (x has to be between 0 and 1)

float random_value_01()					    { return (float)rand() / RAND_MAX; }			  //Random Value from 0.0 to 1.0.
float random_value_0m(float max)			{ return max * rand() / RAND_MAX; }				  //Random Value from 0.0 to max.
float random_value_mm(float min, float max) { return min + (max - min) * rand() / RAND_MAX; } //Random Value from min to max.

//Neural Network
struct neuralnet* allocate_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	
	struct neuralnet* n = malloc(sizeof(struct neuralnet));

	n->input_count = input_count;
	n->hidden_layer_count = hidden_layer_count;
	n->neurons_per_hidden_layer = neurons_per_hidden_layer;
	n->output_count = output_count;

	n->edges_count = (input_count + 1) * neurons_per_hidden_layer + 
		(neurons_per_hidden_layer + 1) * neurons_per_hidden_layer * (hidden_layer_count - 1) + 
		(neurons_per_hidden_layer + 1) * output_count;

	n->edges = (float*)malloc(sizeof(float) *(n->edges_count));

	return n;

}
void initialize_neural_net(struct neuralnet* net){
	
	for (int i = 0; i < net->edges_count; i++){
		net->edges[i] = inverse_sigmoid(random_value_01());
	}

}
struct neuralnet* create_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	
	struct neuralnet* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net(net);
	return net;

}

void deallocate_neural_net(struct neuralnet* net){

	free(net->edges);
	free(net);
	
}
void destroy_neural_net(struct neuralnet* net){
	deallocate_neural_net(net);
}

//Testing and Debugging
int main(void){
	srand((unsigned int)time(NULL));

	printf("%f", inverse_sigmoid(0.0000000000000000000000000000000000002f));
	getchar();

	return 0;
}
