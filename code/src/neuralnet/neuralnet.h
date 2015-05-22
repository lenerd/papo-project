#include <stdlib.h>

#ifndef NEURALNET
#define NEURALNET

#define E 2.7182818284

float sigmoid(float x);							//Mathematical sigmoid function for the intensity of firing neurons. Output values are between 0.0 and 1.0, exclusive. Cuts the y-Axis at 0.5.
float inverse_sigmoid(float x);					//Inverse of the sigmoid function for initial edge weights of the neuralnet (x has to be between 0.0 and 1.0, exclusive)
float centered_sigmoid(float x);				//Centered version of the Sigmoid function. Output values range from -1.0 to 1.0 (exclusive) with the center of the function at (0|0).

float random_value_01();						//Random Value from 0.0 to 1.0.
float random_value_0m(float max);				//Random Value from 0.0 to max.
float random_value_mm(float min, float max);	//Random Value from min to max.

struct neuralnet{
	int edges_count;

	int input_count;
	int hidden_layer_count;
	int neurons_per_hidden_layer;
	int output_count;

	float* edges;
};

struct neuralnet* allocate_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);
void initialize_neural_net_random(struct neuralnet* net);
void initialize_neural_net_buffer(struct neuralnet* net, float* buffer);
void initialize_neural_net_data(struct neuralnet* net, char* filepath);
struct neuralnet* create_neural_net_random(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);
struct neuralnet* create_neural_net_buffer(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, float* edges);
struct neuralnet* create_neural_net_data(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, char* filepath);

void deallocate_neural_net(struct neuralnet* net);
void destroy_neural_net(struct neuralnet* net);

void calculate_output(struct neuralnet* net, float* input, float* output); //Calculates the output of the neural net based on input.

void print_neural_net(struct neuralnet* net); //Print the edge weights to console

#endif