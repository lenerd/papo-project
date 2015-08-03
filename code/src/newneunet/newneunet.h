#ifndef NEURALNET_H
#define NEURALNET_H

/**
 * \file
 * \brief Contains everything required for neural networks.
 * \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
 * \ingroup neuralnet
 */

#include <stddef.h>
#include <stdint.h>
#include "util/math_ext.h"

/**
 * \brief Represents a neural network.
 */
typedef struct
{
    /** \brief Number of layers.*/
    size_t layer_count;

    /**
     * \brief Array containing layer sizes.
     *
     * Length of array equals layer_count.
     */
    size_t* neurons_per_layer;

    /**
     * \brief Buffer containing all edge weights.
     *
     * TODO: interface description
     */
    float* edge_buf;

    /**
     * \private
     * Helper to implement the edges 3D-array.
     * Do not use this otherwise!
     */
    float** edge_helper;

    /**
     * \brief 3D-Interface for the edges.
     *
     * edges[x][y][z] = edge weight between neuron y on layer x and neuron z on
     * layer x + 1.
     * * 0 <= x < layer_count
     * * 0 <= y < neurons_per_layer[x]
     * * 0 <= z < neurons_per_layer[x+1]
     */
    float*** edges;

} neuralnet_t;


/**
 * Calculates the amount of edges in a neuralnetwork with given properties.
 *
 * \f[
 * \sum_{l = 0}^{layer\_count - 2} neurons\_per\_layer[l] \cdot
 * neurons\_per\_layer[l+1]
 * \f]
 * \param layer_count       		Number of layers.
 * \param neurons_per_layer_count 	Array containing layer sizes.
 * \return Number of edges.
 * \pre layer_count > 1
 * \pre neurons_per_layer != NULL
 * \pre len(neurons_per_layer) == layer_count
 */
size_t edge_count (size_t layer_count, const size_t* neurons_per_layer);

/**
 * Calculates the amount of nodes in a neuralnetwork with given properties.
 *
 * \f[
 * \sum_{l = 0}^{layer\_count - 1} neurons\_per\_layer[l]
 * \f]
 * \param layer_count       		Number of layers.
 * \param neurons_per_layer_count 	Array containing layer sizes.
 * \return Number of nodes.
 * \pre layer_count > 1
 * \pre neurons_per_layer != NULL
 * \pre len(neurons_per_layer) == layer_count
 */
size_t node_count (size_t layer_count, const size_t* neurons_per_layer);

/**
 * \brief Creates and returns a neuralnet with random edge-weights, given its
 * preferences.
 *
 * This allocates and initializes a new neural net with the given preferences.
 * \param layer_count       		Number of layers.
 * \param neurons_per_layer_count 	Number of neurons in each layer.
 * \return Pointer to the new neural network.
 * \pre layer_count > 1
 * \pre len(neurons_per_layer) == layer_count
 * \pre neurons_per_layer_count[x] > 0, where 0 <= x < layer_count
 */
neuralnet_t* create_neural_net_random (const size_t layer_count,
                                       const size_t* neurons_per_layer);

/**
 * \brief Creates and returns a neuralnet with given edge-weights.
 *
 * This allocates and initializes a new neural net.
 * \param layer_count       		Number of hidden layers.
 * \param neurons_per_layer_count 	Number of neurons in each hidden layer.
 * \return Pointer to the new neural network.
 * \pre layer_count > 1
 * \pre len(neurons_per_layer) == layer_count
 * \pre neurons_per_layer != NULL
 * \pre neurons_per_layer[x] > 0, where 0 <= x < layer_count
 * \pre edges != NULL
 * \pre len(edges) == edge_count
 */
neuralnet_t* create_neural_net_buffer (const size_t layer_count,
                                       const size_t* neurons_per_layer,
                                       float* edges);

/**
 * \brief Saves a neural network in a file;
 *
 * If the file already exists, it will be overwritten.
 * The output buffer will not be saved.
 * TODO: format specification
 *
 * \param net    Pointer to the new neural network.
 * \param path   Path to a file.
 * \param binary Is the file in a binary format?
 * \pre net != NULL
 * \pre path != NULL
 * \pre User is permitted to write in the path.
 * \post The state of the neural network is saved in the file.
 */
void neural_net_to_file (neuralnet_t* net, const char* path, bool binary);

/**
 * \brief Creates and returns a neuralnet with edge-weights stored in a file.
 *
 * This allocates and initializes a new neural net with the data stored in given
 * file.
 * TODO: format specification
 *
 * \param path   Path to a file containing edge weights.
 * \param binary Is the file in a binary format?
 * \return Pointer to the new neural network.
 * \pre path != NULL
 * \pre The file specified by path exists, is readable and contains data in the
 * correct format.
 * \post Returned network is initialized with the data from the file.
 */
neuralnet_t* neural_net_from_file (const char* path, bool binary);

/**
 * \brief Destroys a neuralnet and frees all used resources.
 * \param net Neural network to destroy.
 * \pre net != NULL
 * \post All used memory is freed.
 */
void destroy_neural_net (neuralnet_t* net);

/**
 * \brief Calculates the output of a given neuralnet and input.
 * \param net Network to be used
 * \param input Buffer containing input values of a given type.
 * \pre net != NULL
 * \pre input != NULL
 * \pre length(input) = net->neurons_per_layer[0]
 */
float* calculate_output (const neuralnet_t* net, const float* input);

/**
 * \brief Trains the neuralnet through backpropagation.
 * \param net Network to be used
 * \param target_output Buffer containing target values the neuralnet should
 * have computed.
 * \pre net != NULL
 * \pre input != NULL
 * \pre target_output != NULL
 * \pre length(*input) = net->input_count
 * \pre length(*target_output) = net->output_count
 */
void backpropagate (neuralnet_t* net, const float* input,
                    const float* target_output);

/**
 * \brief Print the edge weights to console. TH stands for threashold, EWs are
 * the edge weights. Each row stands for one receiving neuron with it's TH and
 * receiving EWs.
 * \pre net != NULL
 */
void print_neural_net (const neuralnet_t* net);

#endif /* NEURALNET_H */
