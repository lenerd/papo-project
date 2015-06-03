#include <stdio.h>
#include <stdlib.h>
#include "neuralnet/neuralnet.h"
#include "math_extend/math_ext.h"

int edge_count(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	return (input_count + 1) * neurons_per_hidden_layer +
		(neurons_per_hidden_layer + 1) * neurons_per_hidden_layer * (hidden_layer_count - 1) +
		neurons_per_hidden_layer * output_count;
}

neuralnet* allocate_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	
	neuralnet* n = (neuralnet*)malloc(sizeof(neuralnet));

	n->input_count = input_count;
	n->hidden_layer_count = hidden_layer_count;
	n->neurons_per_hidden_layer = neurons_per_hidden_layer;
	n->output_count = output_count;

	n->edges_count = edge_count(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);

	n->edges = (float*)malloc(sizeof(float) *(n->edges_count));

	return n;

}

void initialize_neural_net_random(neuralnet* net){
	
	for (int i = 0; i < net->edges_count; i++){
		net->edges[i] = inverse_sigmoid(random_value_01());
	}

}
void initialize_neural_net_buffer(neuralnet* net, float* edges){

	for (int i = 0; i < net->edges_count; i++){
		net->edges[i] = edges[i];
	}

}
void initialize_neural_net_data(neuralnet* net, char* filepath){
	//TODO: implement
}

neuralnet* create_neural_net_random(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	
	neuralnet* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_random(net);
	return net;

}
neuralnet* create_neural_net_buffer(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, float* edges){

	neuralnet* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_buffer(net, edges);
	return net;

}
neuralnet* create_neural_net_data(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, char* filepath){

	neuralnet* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_data(net, filepath);
	return net;

}

void deallocate_neural_net(neuralnet* net){

	free(net->edges);
	free(net);
	
}

void destroy_neural_net(neuralnet* net){
	deallocate_neural_net(net);
}

void calculate_output(const neuralnet* net, float* input, float* output){ //Tons of parallel optimization possibilities.
	
	float* old_current = (float*)malloc(sizeof(float) * net->neurons_per_hidden_layer);
	float* current = (float*)malloc(sizeof(float) * net->neurons_per_hidden_layer);

	float sum = 0.0f;
	int index = 0;

	//Input layer to hidden layer calculation
	for (int i = 0; i < net->neurons_per_hidden_layer; i++){
		sum = -net->edges[index]; //Bias (threshhold simulated as edge)
		index++;
		for (int j = 0; j < net->input_count; j++, index++){
			sum += net->edges[index] * input[i];
		}
		current[i] = sum > 0.0f ? centered_sigmoid(sum): 0.0f;
	}

	//Hidden layer intern calculation
	for (int i = 0; i < net->hidden_layer_count - 1; i++){
		int c = (int)old_current;
		old_current = current;
		current = (float*)c;
		for (int j = 0; j < net->neurons_per_hidden_layer; j++){
			sum = -net->edges[index]; //Bias (threshhold simulated as edge)
			index++;
			for (int k = 0; k < net->neurons_per_hidden_layer; k++, index++){
				sum += net->edges[index] * old_current[k];
			}
			current[i] = sum > 0.0f ? centered_sigmoid(sum) : 0.0f;
		}
	}

	//Hidden layer to output layer
	for (int i = 0; i < net->output_count; i++){
		for (int j = 0; j < net->neurons_per_hidden_layer; j++, index++){
			sum += net->edges[index] * current[j];
		}
		output[i] = sum;
	}

	free(old_current);
	free(current);
}

void print_neural_net(const neuralnet* net){

	int index = 0;

	printf("Neural Net Edge Weights:");

	//Input layer to hidden layer calculation
	printf("\n\n\tInput layer to hidden layer:\n");
	for (int i = 0; i < net->neurons_per_hidden_layer; i++){
		//Threashold and incoming edge weights of i-th neuron
		printf("\n\t\tTH: %+.2f EWs:", net->edges[index]);
		index++;
		for (int j = 0; j < net->input_count; j++, index++){
			printf(" %+.2f", net->edges[index]);
		}
	}

	//Hidden layer intern calculation
	printf("\n\n\tHidden layer intern:");
	for (int i = 0; i < net->hidden_layer_count - 1; i++){
		printf("\n");
		for (int j = 0; j < net->neurons_per_hidden_layer; j++){
			//Threashold and incoming edge weights of j-th neuron
			printf("\n\t\tTH: %+.2f EWs:", net->edges[index]);
			index++;
			for (int k = 0; k < net->neurons_per_hidden_layer; k++, index++){
				printf(" %+.2f", net->edges[index]);
			}
		}
	}

	//Hidden layer to output layer
	printf("\n\n\tHidden layer to output layer:\n");
	for (int i = 0; i < net->output_count; i++){
		//Threashold and incoming edge weights of i-th neuron
		printf("\n\t\tEWs:");
		for (int j = 0; j < net->neurons_per_hidden_layer; j++, index++){
			printf(" %+.2f", net->edges[index]);
		}
	}
	printf("\n\n\n");

}
