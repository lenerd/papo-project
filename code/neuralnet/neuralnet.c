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

	n->inputCount = inputCount;
	n->hiddenLayerCount = hiddenLayerCount;
	n->neuronsPerHiddenLayer = neuronsPerHiddenLayer;
	n->outputCount = outputCount;

	n->inputEdges = (float**)malloc(sizeof(float*) * inputCount);
	for (int i = 0; i < inputCount; i++){
		n->inputEdges[i] = (float*)malloc(sizeof(float) * neuronsPerHiddenLayer);
	}

	n->edges = (float***)malloc(sizeof(float**) * neuronsPerHiddenLayer);
	for (int i = 0; i < neuronsPerHiddenLayer; i++){
		n->edges[i] = (float**)malloc(sizeof(float*) * neuronsPerHiddenLayer);
		for (int j = 0; j < neuronsPerHiddenLayer; j++){
			n->edges[i][j] = (float*)malloc(sizeof(float) * (hiddenLayerCount - 1));
		}
	}

	n->outputEdges = (float**)malloc(sizeof(float*) * neuronsPerHiddenLayer);
	for (int i = 0; i < neuronsPerHiddenLayer; i++){
		n->outputEdges[i] = (float*)malloc(sizeof(float) * outputCount);
	}

	return n;
}
void initialize_neural_net(struct neuralnet* net){
	
	for (int i = 0; i < net->inputCount; i++){
		for (int j = 0; j < net->neuronsPerHiddenLayer; j++){
			net->inputEdges[i][j] = inverseSigmoid(randomValue01());
		}
	}

	for (int i = 0; i < net->neuronsPerHiddenLayer; i++){
		for (int j = 0; j < net->neuronsPerHiddenLayer; j++){
			for (int k = 0; k < net->hiddenLayerCount - 1; k++){
				net->edges[i][j][k] = inverseSigmoid(randomValueMM(-1.0f, 1.0f));
			}			
		}
	}

	for (int i = 0; i < net->neuronsPerHiddenLayer; i++){
		for (int j = 0; j < net->outputCount; j++){
			net->outputEdges[i][j] = inverseSigmoid(randomValueMM(-1.0f, 1.0f));
		}
	}
}
struct neuralnet* create_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	struct neuralnet* net = allocateNeuralNet(inputCount, hiddenLayerCount, neuronsPerHiddenLayer, outputCount);
	initializeNeuralNet(net);
	return net;
}

void deallocate_neural_net(struct neuralnet* net){

	for (int i = 0; i < net->inputCount; i++){
		free(net->inputEdges[i]);
	}
	free(net->inputEdges);

	for (int i = 0; i < net->neuronsPerHiddenLayer; i++){
		for (int j = 0; j < net->neuronsPerHiddenLayer; j++){
			free(net->edges[i][j]);
		}
		free(net->edges[i]);
	}
	free(net->edges);

	for (int i = 0; i < net->neuronsPerHiddenLayer; i++){
		free(net->outputEdges[i]);
	}
	free(net->outputEdges);
}
void destroy_neural_net(struct neuralnet* net){
	deallocateNeuralNet(net);
}

//Testing and Debugging
int main(int argc, char** argv){
	srand((unsigned int)time(NULL));

	printf("%f", inverseSigmoid(0.0000000000000000000000000000000000002f));
	getchar();

	return 0;
}