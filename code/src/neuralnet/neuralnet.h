#ifndef NEURALNET_H
#define NEURALNET_H

/**
* \file
* \brief Contains everything required for neural networks.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \ingroup neuralnet
*/


#include <stdint.h>


/**
* \brief Represents a neural network.
*/
typedef struct
{
	/** \brief Total count of all edges.*/
	uint32_t edges_count;
	/** \brief Count of input neurons.*/
	uint32_t input_count;
	/** \brief Count of hidden layers.*/
	uint32_t hidden_layer_count;
	/** \brief Count of neurons each hidden layer has.*/
	uint32_t neurons_per_hidden_layer;
	/** \brief Count of output neurons.*/
	uint32_t output_count;

	/** \brief Array of floats to store the calculated output.*/
	float* output;

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
uint32_t edge_count(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count);

/**
* \brief Creates and returns a neuralnet with random edge-weights, given its preferences. This essentially does nothing but allocates and initializes a new neural net.
* \pre input_count > 0
* \pre hidden_layer_count > 0
* \pre neurons_per_hidden_layer > 0
* \pre output_count > 0
*/
neuralnet_t* create_neural_net_random(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count);

/**
* \brief Creates and returns a neuralnet with given edge-weights in form of an float array, given its preferences. This essentially does nothing but allocates and initializes a new neural net.
* \pre input_count > 0
* \pre hidden_layer_count > 0
* \pre neurons_per_hidden_layer > 0
* \pre output_count > 0
* \pre edges != NULL
*/
neuralnet_t* create_neural_net_buffer(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count, float* edges);

/**
* \brief Creates and returns a neuralnet with given edge-weights in form of a data, given its preferences. This essentially does nothing but allocates and initializes a new neural net.
* \pre input_count > 0
* \pre hidden_layer_count > 0
* \pre neurons_per_hidden_layer > 0
* \pre output_count > 0
* \pre filepath != NULL
*/
neuralnet_t* create_neural_net_data(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count, char* filepath);

/**
* \brief Removes and cleans up a given neuralnet. Essentially this does nothing else than deallocating the neuralnet. If there will be further means to clean up a neuralnet, all those functions should be summarized here.
* \pre net =! NULL
*/
void destroy_neural_net(neuralnet_t* net);

/**
* \brief Calculates and returns output of a given neuralnet and input.
* \pre net != NULL
* \pre input != NULL
*/
float* calculate_output(const neuralnet_t* net, float* input);

/**
* \brief Print the edge weights to console. TH stands for threashold, EWs are the edge weights. Each row stands for one receiving neuron with it's TH and receiving EWs.
* \pre net != NULL
*/
void print_neural_net(const neuralnet_t* net);

#endif /* NEURALNET_H */
