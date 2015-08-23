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
#include <stddef.h>
#include <stdint.h>


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
 * \brief Sigmoid function.
 *
 * Maps a floating point number to a value between 0.0 and 1.0. Cuts the y-Axis
 *at 0.5.
 *
 * Definition:
 * \f[
 * S(x) = \frac{1}{1 + e^{-x}}
 * \f]
 * Edge cases: \f$S(-\infty) = 0\f$ and \f$S(\infty) = 1\f$.
 * \param x \f$x\f$
 * \return \f$S(x)\f$
 * \post \f$0 \leq S(x) \leq 1\f$
 */
float sigmoid (float x);

/**
 * \brief Inverse of the sigmoid function.
 *
 * Definition:
 * \f[
 * S^{-1}(y) = - \ln \left( \frac{1}{y} - 1 \right)
 * \f]
 * Edge cases: \f$S^{-1}(0) = -\infty\f$ and \f$S^{-1}(1) = \infty\f$.
 *
 * \param y \f$y\f$
 * \return \f$S^{-1}(y)\f$
 * \pre 0 <= y <= 1
 */
float inverse_sigmoid (float y);

/**
 * \brief Centered version of the Sigmoid function. Output values range from
 * -1.0
 * to 1.0 (exclusive) with the center of the function at (0|0).
 */
float centered_sigmoid (float x);

/**
 * \brief Applies the sigmoid function to an array.
 *
 * Creates a new array.
 * \param array Array to work on.
 * \param size  Size of the array.
 * \return sigmoid(array)
 * \pre array != NULL
 */
float* sigmoidize (const float* array, const size_t size);

/**
 * \brief Applies the inverse sigmoid function to an array.
 *
 * Creates a new array.
 * \param array Array to work on.
 * \param size  Size of the array.
 * \return sigmoid^-1(array)
 * \pre array != NULL
 */
float* desigmoidize (const float* array, const size_t size);

/**
 * \brief Applies the sigmoid function to an array.
 *
 * Works inplace.
 * \param array Array to work on.
 * \param size  Size of the array.
 * \return sigmoid(array)
 * \pre array != NULL
 * \post array <- sigmoid(array)
 */
float* sigmoidize_inplace (float* array, const size_t size);

/**
 * \brief Applies the inverse sigmoid function to an array.
 *
 * Works inplace.
 * \param array Array to work on.
 * \param size  Size of the array.
 * \return sigmoid^-1(array)
 * \pre array != NULL
 * \post array <- sigmoid^-1(array)
 */
float* desigmoidize_inplace (float* array, const size_t size);

/**
* \brief Computer the factorial of a given number
* 
* Works in place.
* \param number the number whose factorial should be computed
*/
uint64_t factorial (uint64_t number);

#endif /* MATH_EXT_H */
