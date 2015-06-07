#ifndef MATH_EXT_H
#define MATH_EXT_H

/**
* \file
* \brief Extension functions for random and mathematical calculation.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \author Lennart Braun <3braun@informatik.uni-hamburg.de>
* \ingroup math
*/


#include <stdbool.h>


/**
 * \brief Flag, whether the random number generator has been seeded yet.
 */
bool seed_set;

/**
 * \brief Sets the seed for the random number generator.
 * \param seed Seed to be used.
 * \post seed_set == true
 */
void set_seed (unsigned int seed);
/**
 * \brief Seeds the random number generator with the system time.
 * timestamp.
 * \post seed_set == true
 */
void set_seed_time (void);

/**
 * \brief Seeds the random number generator with the system time, if not already
 * done.
 * \post seed_set == true
 */
void set_seed_ifn ();

/**
 * \brief Returns a random float between 0 and 1.
 * \return Random value.
 * \post 0 <= return <= 1
 */
float random_value_01 ();

/**
 * \brief Returns a random float between 0 and max.
 * \param max Maximum value for the generated value;
 * \return Random value.
 * \pre max >= 0
 * \post 0 <= return <= max
 */
float random_value_0m (float max);

/**
 * \brief Returns a random float between min and max.
 * \param min Minimum value for the generated value;
 * \param max Maximum value for the generated value;
 * \return Random value.
 * \pre min <= max
 * \post min <= return <= max
*/
float random_value_mm (float min, float max);

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
