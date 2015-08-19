#ifndef UTIL_H
#define UTIL_H

/**
 * \file
 * \brief Contains miscellanious useful functions.
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 */


#include <stdint.h>
#include <stdlib.h>
#include <time.h>


/**
 * \brief Allocates a buffer with error checking.
 *
 * Uses malloc for allocation.
 *
 * \param size Length of requested memory in bytes.
 * \param file Filename to display in error message.
 * \param line Line to display in error message.
 * \pre size != 0
 * \return Pointer to allocated buffer.
 * \post Allocation successfull or exit with error message.
 */
void* safe_malloc (size_t size, const char* file, unsigned long line);


/**
 * \brief Allocates a buffer with error checking.
 *
 * Uses calloc for allocation.
 *
 * \param nmemb Number of elements.
 * \param size Length of one element.
 * \param file Filename to display in error message.
 * \param line Line to display in error message.
 * \pre nmemb != 0
 * \pre size != 0
 * \return Pointer to allocated buffer.
 * \post Allocation successfull or exit with error message.
 */
void* safe_calloc (size_t nmemb, size_t size, const char* file,
                   unsigned long line);


/**
 * \brief Reallocates a buffer with error checking.
 *
 * Uses realloc for allocation.
 *
 * \param ptr  Pointer to the original buffer.
 * \param size Length of requested memory in bytes.
 * \param file Filename to display in error message.
 * \param line Line to display in error message.
 * \pre size != 0
 * \return Pointer to allocated buffer.
 * \post Allocation successfull or exit with error message.
 */
void* safe_realloc (void* ptr, size_t size, const char* file,
                    unsigned long line);


/**
 * \brief Shortcut for safe_malloc().
 * \param size Length of requested memory in bytes.
 */
#define SAFE_MALLOC(size) safe_malloc(size, __FILE__, __LINE__)


/**
 * \brief Shortcut for safe_calloc().
 * \param nmemb Number of elements.
 * \param size Length of one element.
 */
#define SAFE_CALLOC(nmemb, size) safe_calloc(nmemb, size, __FILE__, __LINE__)


/**
 * \brief Shortcut for safe_calloc().
 * \param ptr  Pointer to an allocated buffer.
 * \param size Length of requested memory in bytes.
 */
#define SAFE_REALLOC(ptr, size) safe_realloc(ptr, size, __FILE__, __LINE__)

/**
 * \brief TODO: documentation
 * \param buf1
 * \param buf2
 */
void swap_int_buffer(int** buf1, int** buf2);

/**
 * \brief TODO: documentation
 * \param buf1
 * \param buf2
 */
void swap_float_buffer(float** buf1, float** buf2);

/**
 * \brief Maximum size over array of sizes.
 * \param array Buffer to work on.
 * \param n Length of buffer.
 * \return Value of maximum element.
 * \pre array != NULL
 */
size_t max_size (const size_t* array, size_t n);

/**
 * \brief Calculates the difference between two timestamps.
 * \param start First timestamp.
 * \param end Second timestamp.
 * \return end - start
 */
struct timespec diff_timespec (struct timespec start, struct timespec end);

/**
 * \brief Calculates the sum between two timestamps.
 * \param start First timestamp.
 * \param end Second timestamp.
 * \return t1 + t2
 */
struct timespec sum_timespec (struct timespec t1, struct timespec t2);

/**
 * \brief Calculates the quotient of a time and an integer
 * \param t First timestamp.
 * \param d divisor
 * \return t / d
 */
struct timespec div_timespec (struct timespec t, uint64_t d);

/**
 * \brief Prints time to stdout.
 *
 * Time in seconds with nanosecond precision.
 * Format: \\d+\\\.\\d{9}
 * \param time Time to print.
 */
void print_time (struct timespec time);

#endif /* UTIL_H */
