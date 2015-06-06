#ifndef MATH_EXT_H
#define MATH_EXT_H

/**
* \file
* \brief Extension functions for random and mathematical calculation.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \ingroup math
*/


/**
* \brief Represents wether the seed for random values have been set, yet (1 for true and 0 for false).
*/
int seed_set;

/**
* \brief Sets the seed for random number generation to the time.
*/
void set_seed();

/**
* \brief Runs set_seed, if the seed hasn't been set already.
*/
void set_seed_ifn();

/**
* \brief Returns a random float between 0 and 1.
*/
float random_value_01();

/**
* \brief Returns a random float between 0 and max.
* \pre max > 0
*/
float random_value_0m(float max);

/**
* \brief Returns a random float between min and max.
* \pre max > min
*/
float random_value_mm(float min, float max);

/**
* \brief Mathematical sigmoid function. Output values are between 0.0 and 1.0, exclusive. Cuts the y-Axis at 0.5.
*/
float sigmoid(float x);

/**
* \brief Inverse of the sigmoid function (x has to be between 0.0 and 1.0, exclusive).
* \pre x > 0 && x < 1
*/
float inverse_sigmoid(float x);

/**
* \brief Centered version of the Sigmoid function. Output values range from -1.0 to 1.0 (exclusive) with the center of the function at (0|0).
*/
float centered_sigmoid(float x);

#endif /* MATH_EXT_H */
