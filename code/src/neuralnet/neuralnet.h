#ifndef NEURALNET_H
#define NEURALNET_H

/**
* \file
* \brief Contains everything required for neural networks.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \author Lennart Braun <3braun@informatik.uni-hamburg.de>
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

    /**
     * \brief Buffer containing all edge weights.
     *
     * The buffer is partioned as following:
     * - (input_count + 1) X neurons_per_hidden_layer
     * - hidden_layer_count X (neurons_per_hidden_layer + 1)
     *                      X neurons_per_hidden_layer
     * - neurons_per_hidden_layer X output_count
     */
    float* edge_buf;

    /**
     * \brief 2D-Interface for the edges between input and first hidden layer.
     *
     * Dimensions: (neurons_per_hidden_layer) X (neurons_per_hidden_layer)
     *
     * edges[x][y] = edge weight between input neuron x and neuron y on the
     * first hidden layer
     */
    float** input_edges;

    /**
     * \brief 3D-Interface for the edges between the hidden layers
     *
     * Dimensions: (hidden_layer_count - 1) X (neurons_per_hidden_layer)
     *                                      X (neurons_per_hidden_layer)
     *
     * edges[x][y][z] = edge weight between neuron y on layer x and neuron z on
     *                  layer x + 1
     */
    float*** hidden_edges;

    /**
     * \brief 2D-Interface for the edges between last hidden and output layer.
     *
     * Dimensions: (neurons_per_hidden_layer) X (neurons_per_hidden_layer)
     *
     * edges[x][y] = edge weight between input neuron x and neuron y on the
     * first hidden layer
     */
    float** output_edges;

    /** \brief Buffer to store some pointers (required for magic). */
    float** edge_helper_buf;

} neuralnet_t;

/**
 * \brief Returns how many total edges there will be in a neuralnet, based on
 * given preferences.
 * \pre input_count > 0
 * \pre hidden_layer_count > 0
 * \pre neurons_per_hidden_layer > 0
 * \pre output_count > 0
 */
uint32_t edge_count (uint32_t input_count, uint32_t hidden_layer_count,
                     uint32_t neurons_per_hidden_layer, uint32_t output_count);

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
neuralnet_t* create_neural_net_random (uint32_t input_count,
                                       uint32_t hidden_layer_count,
                                       uint32_t neurons_per_hidden_layer,
                                       uint32_t output_count);

/**
 * \brief Creates and returns a neuralnet with given edge-weights.
 *
 * This allocates and initializes a new neural net with the given preferences.
 * \param input_count              Number of input neurons.
 * \param hidden_layer_count       Number of hidden layers.
 * \param neurons_per_hidden_layer Number of neurons in each hidden layer.
 * \param output_count             Number of output neurons.
 * \param edges                    Buffer containing edge weights
 * \return Pointer to the new neural network.
 * \pre input_count > 0
 * \pre hidden_layer_count > 0
 * \pre neurons_per_hidden_layer > 0
 * \pre output_count > 0
 * \pre edges != NULL
 * \pre length(edges) = edge_count
 * \post Returned network is initialized with given edge weigths.
 */
neuralnet_t* create_neural_net_buffer (uint32_t input_count,
                                       uint32_t hidden_layer_count,
                                       uint32_t neurons_per_hidden_layer,
                                       uint32_t output_count, float* edges);

/**
 * \brief Creates and returns a neuralnet with edge-weights stored in a file.
 *
 * This allocates and initializes a new neural net with the given preferences.
 * The file format is a binary sequence of floats stored as little endian.
 *
 * \param input_count              Number of input neurons.
 * \param hidden_layer_count       Number of hidden layers.
 * \param neurons_per_hidden_layer Number of neurons in each hidden layer.
 * \param output_count             Number of output neurons.
 * \param path                     Path to a file containing edge weights.
 * \return Pointer to the new neural network.
 * \pre input_count > 0
 * \pre hidden_layer_count > 0
 * \pre neurons_per_hidden_layer > 0
 * \pre output_count > 0
 * \pre path != NULL
 * \pre The file speicified by path exists, is readable and contains data in the
 *correct format.
 * \post Returned network is initialized with given edge weigths.
 */
neuralnet_t* create_neural_net_file (uint32_t input_count,
                                     uint32_t hidden_layer_count,
                                     uint32_t neurons_per_hidden_layer,
                                     uint32_t output_count, char* path);

/**
 * \brief Destroys a neuralnet and frees all used ressources.
 * \param net Neural network to destroy.
 * \pre net =! NULL
 * \post All used memory is freed.
 */
void destroy_neural_net (neuralnet_t* net);

/**
 * \brief Calculates the output of a given neuralnet and input.
 * \param net Network to be used
 * \param input Buffer containing input values.
 * \pre net != NULL
 * \pre input != NULL
 * \pre length(input) = net->input_count
 * \post net->output is updated
 */
float* calculate_output (neuralnet_t* net, float* input);

/**
 * \brief Print the edge weights to console. TH stands for threashold, EWs are
 * the edge weights. Each row stands for one receiving neuron with it's TH and
 * receiving EWs.
 * \pre net != NULL
 */
void print_neural_net (const neuralnet_t* net);

#endif /* NEURALNET_H */
