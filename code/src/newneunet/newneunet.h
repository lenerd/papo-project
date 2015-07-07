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
	
	/** \brief Number of input neurons.*/
	uint32_t input_count;
	/** \brief Number of hidden layers.*/
	uint32_t hidden_layer_count;
	/** \brief Number of neurons each hidden layer has.*/
	uint32_t neurons_per_hidden_layer;
	/** \brief Number of output neurons.*/
	uint32_t output_count;

	/**
	* \brief Buffer containing all input to first hidden layer weights.
	*/
	float** input_weights;
	/**
	* \brief Buffer containing all internal hidden layer weights.
	*/
	float*** hidden_weights;
	/**
	* \brief Buffer containing all last hidden to output layer weights.
	*/
	float** output_weights;

} neuralnet_t;

/**
 * \brief Creates and returns a neuralnet with random edge-weights, given its
 * preferences.
 *
 * This allocates and initializes a new neural net with the given preferences.
 * \param input_count              Number of input neurons.
 * \param hidden_layer_count       Number of hidden layers.
 * \param neurons_per_hidden_layer Number of neurons in each hidden layer.
 * \param output_count             Number of output neurons.
 * \return Pointer to the new neural network.
 * \pre input_count > 0
 * \pre hidden_layer_count > 0
 * \pre neurons_per_hidden_layer > 0
 * \pre output_count > 0
 */
neuralnet_t* create_neural_net_random(uint32_t input_count, uint32_t hidden_layer_count, uint32_t neurons_per_hidden_layer, uint32_t output_count);

/**
 * \brief Destroys a neuralnet and frees all used resources.
 * \param net Neural network to destroy.
 * \pre net != NULL
 * \post All used memory is freed.
 */
void destroy_neural_net(neuralnet_t* net);

/**
 * \brief Calculates the output of a given neuralnet and input.
 * \param net Network to be used
 * \param input Buffer containing input values of a given type.
 * \pre net != NULL
 * \pre input != NULL
 * \pre length(input) = net->input_count
 */
float* calculate_output(neuralnet_t* net, float* input);

/**
 * \brief Trains the neuralnet through backpropagation.
 * \param net Network to be used
 * \param target_output Buffer containing target values the neuralnet should have computed. 
 * \pre net != NULL
 * \pre length(*input) = net->input_count
 * \pre length(*target_output) = net->output_count
 */
void backpropagate(neuralnet_t* net, float** input, float** target_output);

/**
 * \brief Print the edge weights to console. TH stands for threashold, EWs are
 * the edge weights. Each row stands for one receiving neuron with it's TH and
 * receiving EWs.
 * \pre net != NULL
 */
void print_neural_net (const neuralnet_t* net);

#endif
