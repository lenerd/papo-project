#ifndef NEWNEUNET_H
#define NEWNEUNET_H

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
typedef struct{
	
	/** \brief Number of layers.*/
	uint32_t layer_count;
	/** \brief Number of neurons each layer has.*/
	uint32_t* neurons_per_layer_count;

	/**
	* \brief Buffer containing all weights.
	*/
	float*** weights;
	
} neuralnet_t;

/**
 * \brief Creates and returns a neuralnet with random edge-weights, given its
 * preferences.
 *
 * This allocates and initializes a new neural net with the given preferences.
 * \param layer_count       		Number of hidden layers.
 * \param neurons_per_layer_count 	Number of neurons in each hidden layer.
 * \return Pointer to the new neural network.
 * \pre layer_count > 1
 * \pre neurons_per_layer_count[x] > 0, where 0 < x < layer_count
 */
neuralnet_t* create_neural_net_random_new(const uint32_t layer_count, uint32_t* neurons_per_layer_count);

/**
 * \brief Destroys a neuralnet and frees all used resources.
 * \param net Neural network to destroy.
 * \pre net != NULL
 * \post All used memory is freed.
 */
void destroy_neural_net_new(neuralnet_t* net);

/**
 * \brief Calculates the output of a given neuralnet and input.
 * \param net Network to be used
 * \param input Buffer containing input values of a given type.
 * \pre net != NULL
 * \pre input != NULL
 * \pre length(input) = net->input_count
 */
float* calculate_output_new(const neuralnet_t* net, const float* input);

/**
 * \brief Trains the neuralnet through backpropagation.
 * \param net Network to be used
 * \param target_output Buffer containing target values the neuralnet should have computed. 
 * \pre net != NULL
 * \pre length(*input) = net->input_count
 * \pre length(*target_output) = net->output_count
 */
void backpropagate(neuralnet_t* net, const float* input, const float* target_output);

/**
 * \brief Print the edge weights to console. TH stands for threashold, EWs are
 * the edge weights. Each row stands for one receiving neuron with it's TH and
 * receiving EWs.
 * \pre net != NULL
 */
void print_neural_net (const neuralnet_t* net);

#endif
