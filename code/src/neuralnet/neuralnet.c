#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "neuralnet.h"

<<<<<<< HEAD
float sigmoid(float x)		   { return 1.0f / (1.0f + (float)pow(E, -x)); }
float inverse_sigmoid(float x) { return (float)(-log(1.0f / x - 1.0f)); }

=======
float sigmoid(float x)		   { return 1.0f / (1.0f + (float)pow(M_E, -x)); }
float inverse_sigmoid(float x) { return (float)(-log(1.0f / x - 1.0f)); }
>>>>>>> origin/master
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
		neurons_per_hidden_layer * output_count;

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
		printf("\n\t\tEWs:");
		for (int j = 0; j < net->neurons_per_hidden_layer; j++, index++){
			printf(" %+.2f", net->edges[index]);
		}
	}
	printf("\n\n\n");

}

//Testing and Debugging
int main(int argc, char** argv){
	srand((unsigned int)time(NULL));

	

	//1 input, 1 hidden layer, 1 neurons per hidden layer, 1 output.
	struct neuralnet* n1 = allocate_neural_net(1, 1, 1, 1);
	float* edges = (float*)malloc(sizeof(float) * n1->edges_count);
	edges[0] = 0.0f; 
	edges[1] = 1.0f; 
	edges[2] = 1.0f;
	initialize_neural_net_buffer(n1, edges);
	free(edges);

	print_neural_net(n1);
	
	float n1_in[] = { 1.0f };
	float n1_out[1];

	calculate_output(n1, n1_in, n1_out);
	printf("Input: %f\nOutput: %f\n\n", n1_in[0], n1_out[0]);

	//3 input, 4 hidden layer, 6 neurons per hidden layer, 2 output.
	struct neuralnet* n2 = create_neural_net_random(3, 4, 6, 2);

	print_neural_net(n2);

	float n2_in[] = { 0.0f, 0.0f, 0.0f };
	float n2_out[2];

	calculate_output(n2, n2_in, n2_out);
	printf("Input: %f, %f, %f\nOutput: %f, %f\n\n", n2_in[0], n2_in[1], n2_in[2], n2_out[0], n2_out[1]);

	//1 input, 1 hidden layer, 1 neurons per hidden layer, 1 output.
	struct neuralnet* n3 = create_neural_net_random(1, 1, 1, 1);

	print_neural_net(n3);

	float n3_in[] = { 0.0f };
	float n3_out[1];

	calculate_output(n3, n3_in, n3_out);
	printf("Input: %f\nOutput: %f\n\n", n3_in[0], n3_out[0]);

	getchar();

	return 0;
}
