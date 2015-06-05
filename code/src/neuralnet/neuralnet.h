#ifndef NEURALNET_H
#define NEURALNET_H

/**
* \file
* \brief Contains everything required for neural networks.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \ingroup go
*/

/**
* \brief Represents a neural network.
*/
typedef struct
{
	/** \brief Total count of all edges.*/
	int edges_count;
	/** \brief Count of input neurons.*/
	int input_count;
	/** \brief Count of hidden layers.*/
	int hidden_layer_count;
	/** \brief Count of neurons each hidden layer has.*/
	int neurons_per_hidden_layer;
	/** \brief Count of output neurons.*/
	int output_count;

	/** \brief Weights of all edges.*/
	float* edges;
} neuralnet_t;

/**
* \brief Returns how many total edges there will be in a neuralnet, based on given preferences.
* \pre input_count > 0
* \pre hidden_layer_count > 0
* \pre neurons_per_hidden_layer > 0
* \pre output_count > 0
*/
int edge_count(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);

/**
* \brief Allocates and returns the start adress of a neuralnet.
* \pre input_count > 0
* \pre hidden_layer_count > 0
* \pre neurons_per_hidden_layer > 0
* \pre output_count > 0
*/
neuralnet_t* allocate_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);

/**
* \brief Initializes a given neuralnet with random edge-weights.
* \pre net != NULL
*/
void initialize_neural_net_random(neuralnet_t* net);

/**
* \brief Initializes a given neuralnet with given edge-weights in form of an float array.
* \pre net != NULL
* \pre edges != NULL
*/
void initialize_neural_net_buffer(neuralnet_t* net, float* edges);

/**
* \brief Initializes a given neuralnet with given edge-weights in form of a data.
* \pre net != NULL
* \pre filepath != NULL
*/
void initialize_neural_net_data(neuralnet_t* net, char* filepath);

/**
* \brief Creates and returns a neuralnet with random edge-weights, given its preferences. This essentially does nothing but allocates and initializes a new neural net.
* \pre input_count > 0
* \pre hidden_layer_count > 0
* \pre neurons_per_hidden_layer > 0
* \pre output_count > 0
*/
neuralnet_t* create_neural_net_random(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);

/**
* \brief Creates and returns a neuralnet with given edge-weights in form of an float array, given its preferences. This essentially does nothing but allocates and initializes a new neural net.
* \pre input_count > 0
* \pre hidden_layer_count > 0
* \pre neurons_per_hidden_layer > 0
* \pre output_count > 0
* \pre edges != NULL
*/
neuralnet_t* create_neural_net_buffer(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, float* edges);

/**
* \brief Creates and returns a neuralnet with given edge-weights in form of a data, given its preferences. This essentially does nothing but allocates and initializes a new neural net.
* \pre input_count > 0
* \pre hidden_layer_count > 0
* \pre neurons_per_hidden_layer > 0
* \pre output_count > 0
* \pre filepath != NULL
*/
neuralnet_t* create_neural_net_data(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, char* filepath);

/**
* \brief Deallocates (frees) the memory a given neuralnet used.
* \pre net != NULL
*/
void deallocate_neural_net(neuralnet_t* net);

/**
* \brief Removes and cleans up a given neuralnet. Essentially this does nothing else than deallocating the neuralnet. If there will be further means to clean up a neuralnet, all those functions should be summarized here.
* \pre net =! NULL
*/
void destroy_neural_net(neuralnet_t* net);

/**
* \brief Calculates and stores output of a given neuralnet and input.
* \pre net != NULL
* \pre input != NULL
* \pre output != NULL
*/
void calculate_output(const neuralnet_t* net, float* input, float* output);

/**
* \brief Print the edge weights to console. TH stands for threashold, EWs are the edge weights. Each row stands for one receiving neuron with it's TH and receiving EWs.
* \pre net != NULL
*/
void print_neural_net(const neuralnet_t* net);

#endif /* NEURALNET_H */
