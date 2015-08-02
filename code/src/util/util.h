#ifndef UTIL_H
#define UTIL_H

/**
 * \file
 * \brief Contains miscellanious useful functions.
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 */


#include <stdlib.h>


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

void swap_int_buffer(int** buf1, int** buf2);
void swap_float_buffer(float** buf1, float** buf2);
