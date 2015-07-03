#ifndef NEURALNET_H
#define NEURALNET_H

/**
* \file
* \brief Contains everything required for neural networks.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \author Lennart Braun <3braun@informatik.uni-hamburg.de>
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
* \ingroup neuralnet
*/


#include <stdbool.h>
#include <stdint.h>

/**
 * \brief Enumeration of number types.
 */
typedef enum{FLOAT, UINT8} type_t;

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
 * \brief Calculates how many edges are required for a neural network of a given
 * size.
 *
 * \f[
 * E(i, f, n, o) = (i + 1) \cdot n + (h - 1) \cdot (n + 1) \cdot n + n \cdot o
 * \f]
 *
 * \param input_count \f$i\f$
 * \param hidden_layer_count \f$h\f$
 * \param neurons_per_hidden_layer \f$n\f$
 * \param output_count \f$o\f$
 * \return \f$E(i, h, n, o)\f$
 */
uint32_t edge_count (uint32_t input_count, uint32_t hidden_layer_count,
                     uint32_t neurons_per_hidden_layer, uint32_t output_count);

/**
 * \brief Setup pointer interface.
 * \param net The neural network to work on.
 * \pre net != NULL
 * \post {input,hidden,outpout}_edges contain valid pointers.
 */
void build_pointer (neuralnet_t* net);

/**
 * \brief Call this, when a neural networks edge buffer is used as a genome and
 * edge_buf points to a new location.
 * \param arg Pointer to a neural network.
 * \pre arg != NULL
 * \post The neural network is updated to use the new buffer.
 */
void update_neuralnet (void* arg);

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
 * \brief Destroys a neuralnet and frees all used ressources.
 * \param net Neural network to destroy.
 * \pre net != NULL
 * \post All used memory is freed.
 */
void destroy_neural_net (neuralnet_t* net);

/**
 * \brief Calculates the output of a given neuralnet and input.
 * \param net Network to be used
 * \param input Buffer containing input values of a given type.
 * \param type The type of the input buffer. 
 * \pre net != NULL
 * \pre input != NULL
 * \pre length(input) = net->input_count
 * \post net->output is updated
 */
float* calculate_output (neuralnet_t* net, void* input, type_t type);

/**
 * \brief Print the edge weights to console. TH stands for threashold, EWs are
 * the edge weights. Each row stands for one receiving neuron with it's TH and
 * receiving EWs.
 * \pre net != NULL
 */
void print_neural_net (const neuralnet_t* net);

/**
* \brief Executes a backpropagation algorithm to train the nets
*/
float* backpropagation(neuralnet_t* net, int board_size, float threshold);

#endif /* NEURALNET_H */
