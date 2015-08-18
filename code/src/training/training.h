#ifndef TRAINING_H
#define TRAINING_H

/**
* \file
* \brief Contains everything required for neural networks.
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
* \ingroup training
*/

#include "go/board.h"
#include <stdio.h>

/**
 * \brief
 */
typedef struct
{
    /** \brief Size of the input buffer. */
    size_t input_size;
    /** \brief Size of the output buffer. */
    size_t output_size;
    /** \brief Input values. */
    float* input;
    /** \brief Targeted, corresponding output values. */
    float* expected;
} training_data_t;

/**
 * \brief A collection of training data records.
 */
typedef struct
{
    /** \brief Length of data. */
    size_t size;
    /** \brief Array of training data. */
    training_data_t** data;
} dataset_t;

/**
 * \brief Creates a dataset.
 * \param size Size of that dataset.
 * \return New dataset.
 */
dataset_t* dataset_create (size_t size);

/**
 * \brief Destroys a dataset.
 * \param set The set to destroy.
 * \pre set != NULL
 * \post The contained training data is also destroyed.
 */
void dataset_destroy (dataset_t* set);

/**
 * \brief Saves a dataset to a file;
 *
 * If the file already exists, it will be overwritten.
 * TODO: format specification
 *
 * \param set    Pointer to the dataset.
 * \param path   Path to a file.
 * \pre set != NULL
 * \pre path != NULL
 * \pre User is permitted to write in the path.
 * \post The state of the dataset is saved in the file.
 */
void dataset_to_file (dataset_t* set, const char* path);

/**
 * \brief Loads a dataset from a file
 *
 * TODO: format specification
 * \param path   Path to a file containing a dataset.
 * \return Pointer to the loaded dataset, NULL if an error occurred.
 * \pre path != NULL
 * \pre The file specified by path exists, is readable and contains data in the
 * correct format.
 */
dataset_t* dataset_from_file (const char* path);

/**
 * \brief Creates training data of given.
 * \pre input_size Count of networks input neurons.
 * \pre ouput_size Count of networks output neurons.
 * \return New training_data_t object.
 */
training_data_t* td_create (size_t input_size, size_t output_size);

/**
 * \brief Destroys a training data object.
 * \param data
 * \pre data != NULL
 * \post All resources are freed.
 */
void td_destroy (training_data_t* data);


/**
 * \brief Creates a random position on a nxn go board and creates the
 * corresponding input and the targeted output.
 * \param n Size of the board.
 * \return training data
 */
training_data_t* td_generate_nxn_nxn (size_t n);

#if 0
/**
* \brief Reads all .sgf files for the specified board size and returns them as one data set including the expected values for the color
*/
dataset_t* generate_training_data(const char* path, int size, color_t color);

/**
* \brief Generates the expected values for this color. If the position is somehow illegal, expected will be zero, else one.
*/
void generate_expected_values(training_data_t* data);

/**
* \brief Converts .sgf to int*
*/
// TODO: replace with generic load_game function
void input_from_file(training_data_t* data, FILE* fp);

/**
 * Create dataset.
 */
dataset_t* create_dataset (size_t size);

/**
 * Destroy dataset.
 */
void destroy_dataset (dataset_t* set);

/**
 * Initialize training data.
 */
void create_training_data (training_data_t* data, int size, color_t color);

/**
 * Destroy training data.
 */
void destroy_training_data (training_data_t* data);
#endif

#endif /* TRAINING_H */
