#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "neuralnet.h"

float sigmoid(float x)		   { return 1.0f / (1.0f + (float)pow(E, -x)); }
float inverse_sigmoid(float x) { return (float)(-log(1.0f / x - 1.0f)); }
float centered_sigmoid(float x) { return 2.0f * (sigmoid(x) - 0.5f); }

float random_value_01()					    { return (float)rand() / RAND_MAX; }
float random_value_0m(float max)			{ return max * rand() / RAND_MAX; }
float random_value_mm(float min, float max) { return min + (max - min) * rand() / RAND_MAX; }

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
void initialize_neural_net_random(struct neuralnet* net){
	
	for (int i = 0; i < net->edges_count; i++){
		net->edges[i] = inverse_sigmoid(random_value_01());
	}

}
void initialize_neural_net_buffer(struct neuralnet* net, float* edges){

	for (int i = 0; i < net->edges_count; i++){
		net->edges[i] = edges[i];
	}

}
void initialize_neural_net_data(struct neuralnet* net, char* filepath){
	//TODO: implement
}
struct neuralnet* create_neural_net_random(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	
	struct neuralnet* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_random(net);
	return net;

}
struct neuralnet* create_neural_net_buffer(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, float* edges){

	struct neuralnet* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_buffer(net, edges);
	return net;

}
struct neuralnet* create_neural_net_data(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, char* filepath){

	struct neuralnet* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_data(net, filepath);
	return net;

}

void deallocate_neural_net(struct neuralnet* net){

	free(net->edges);
	free(net);
	
}
void destroy_neural_net(struct neuralnet* net){
	deallocate_neural_net(net);
}

void calculate_output(const struct neuralnet* net, float* input, float* output){ //Tons of parallel optimization possibilities.
	
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
		int c = old_current;
		old_current = current;
		current = c;
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
		sum = -net->edges[index]; //Bias (threshhold simulated as edge)
		index++;
		for (int j = 0; j < net->neurons_per_hidden_layer; j++, index++){
			sum += net->edges[index] * current[j];
		}
		output[i] = sum > 0.0f ? centered_sigmoid(sum) : 0.0f;
	}

	free(old_current);
	free(current);
}

void print_neural_net(const struct neuralnet* net){

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
		printf("\n\t\tTH: %+.2f EWs:", net->edges[index]);
		index++;
		for (int j = 0; j < net->neurons_per_hidden_layer; j++, index++){
			printf(" %+.2f", net->edges[index]);
		}
	}
	printf("\n\n\n");

}

//Testing and Debugging
int main(int argc, char** argv){
	srand((unsigned int)time(NULL));

	float edges[] = {	
		1.0f, 2.0f, 3.0f, 4.0f,
		1.1f, 2.1f, 3.1f, 4.1f,
	
		1.0f, 2.0f, 3.0f,
		1.1f, 2.1f, 3.1f,

		1.2f, 2.2f, 3.2f,
		1.3f, 2.3f, 3.3f,
	
		1.0f, 2.0f, 3.0f,
		1.1f, 2.1f, 3.1f,
		1.2f, 2.2f, 3.2f,
		1.3f, 2.3f, 3.3f,
		1.4f, 2.4f, 3.4f};

	//3 input, 3 hidden layer, 2 neurons per hidden layer, 5 output.
	struct neuralnet* n1 = create_neural_net_buffer(3, 3, 2, 5, edges);

	print_neural_net(n1);
	
	float n1_in[] = { 1.0f, 2.0f, 3.0f };
	float n1_out[5];

	calculate_output(n1, n1_in, n1_out);
	printf("Input: %f, %f, %f\nOutput: %f, %f, %f, %f, %f\n\n", n1_in[0], n1_in[1], n1_in[2], n1_out[0], n1_out[1], n1_out[2], n1_out[3], n1_out[4]);

	//6 input, 4 hidden layer, 10 neurons per hidden layer, 2 output.
	struct neuralnet* n2 = create_neural_net_random(6, 4, 10, 2);

	print_neural_net(n2);

	float n2_in[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
	float n2_out[2];

	calculate_output(n2, n2_in, n2_out);
	printf("Input: %f, %f, %f, %f, %f, %f\nOutput: %f, %f\n\n", n2_in[0], n2_in[1], n2_in[2], n2_in[3], n2_in[4], n2_in[5], n2_out[0], n2_out[1]);

	getchar();

	return 0;
}