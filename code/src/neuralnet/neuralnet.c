#include <stdio.h>
#include <stdlib.h>
#include "neuralnet/neuralnet.h"
#include "math_extend/math_ext.h"

int edge_count(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	return (input_count + 1) * neurons_per_hidden_layer +
		(neurons_per_hidden_layer + 1) * neurons_per_hidden_layer * (hidden_layer_count - 1) +
		neurons_per_hidden_layer * output_count;
}

neuralnet_t* allocate_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	
	neuralnet_t* net = NULL;
    net = malloc (sizeof(neuralnet_t));
    if (net == NULL)
    {
        fprintf (stderr, "malloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

	net->input_count = input_count;
	net->hidden_layer_count = hidden_layer_count;
	net->neurons_per_hidden_layer = neurons_per_hidden_layer;
	net->output_count = output_count;

	net->edges_count = edge_count(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);

	net->edges = malloc (sizeof(float) *(net->edges_count));
    if (net->edges == NULL)
    {
        fprintf (stderr, "malloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        free (net);
        exit (EXIT_FAILURE);
    }

	return net;

}

void initialize_neural_net_random(neuralnet_t* net){
	
	for (int i = 0; i < net->edges_count; i++){
		net->edges[i] = inverse_sigmoid(random_value_01());
	}

}
void initialize_neural_net_buffer(neuralnet_t* net, float* edges){

	for (int i = 0; i < net->edges_count; i++){
		net->edges[i] = edges[i];
	}

}
void initialize_neural_net_data(neuralnet_t* net, char* filepath){
	//TODO: implement
}

neuralnet_t* create_neural_net_random(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count){
	
	neuralnet_t* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_random(net);
	return net;

}
neuralnet_t* create_neural_net_buffer(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, float* edges){

	neuralnet_t* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_buffer(net, edges);
	return net;

}
neuralnet_t* create_neural_net_data(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, char* filepath){

	neuralnet_t* net = allocate_neural_net(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
	initialize_neural_net_data(net, filepath);
	return net;

}

void deallocate_neural_net(neuralnet_t* net){

	free(net->edges);
	free(net);
	
}

void destroy_neural_net(neuralnet_t* net){
	deallocate_neural_net(net);
}

void calculate_output(const neuralnet_t* net, float* input, float* output){ //Tons of parallel optimization possibilities.
	
    float* old_current = NULL;
    float* current = NULL;

	old_current = malloc (sizeof(float) * net->neurons_per_hidden_layer);
    if (old_current == NULL)
    {
        fprintf (stderr, "malloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

	current = malloc (sizeof(float) * net->neurons_per_hidden_layer);
    if (current == NULL)
    {
        fprintf (stderr, "malloc() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        free(old_current);
        exit (EXIT_FAILURE);
    }

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

void print_neural_net(const neuralnet_t* net){

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
