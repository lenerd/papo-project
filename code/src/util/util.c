#include "util.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


void* safe_malloc (size_t size, const char* file, unsigned long line)
{
    assert (size != 0);

    void* p = malloc (size);
    if (p == NULL)
    {
        fprintf (stderr, "[%s:%lu] malloc failed (%lu bytes)\n", file, line,
                 size);
        exit (EXIT_FAILURE);
    }
    return p;
}


void* safe_calloc (size_t nmemb, size_t size, const char* file,
                   unsigned long line)
{
    assert (nmemb != 0);
    assert (size != 0);

    void* p = calloc (nmemb, size);
    if (p == NULL)
    {
        fprintf (stderr, "[%s:%lu] calloc failed (%lu bytes)\n", file, line,
                 nmemb * size);
        exit (EXIT_FAILURE);
    }
    return p;
}

void swap_int_buffer(int** buf1, int** buf2){
	int* tmp = *buf1;
	*buf1 = *buf2;
	*buf2 = tmp;
}

void swap_float_buffer(float** buf1, float** buf2){
	float* tmp = *buf1;
	*buf1 = *buf2;
	*buf2 = tmp;
}

float* int_array_to_float(const int* array, const size_t size)
{
    float* result = SAFE_MALLOC (size * sizeof(float));

    for(uint32_t  i = 0; i < size; ++i){
        result[i] = (float)array[i];
    }

    return result;
}

float* uint8_array_to_float(const uint8_t* array, const size_t size)
{
    float* result = SAFE_MALLOC (size * sizeof(float));

    for(uint32_t  i = 0; i < size; ++i){
        result[i] = (float)array[i];
    }

    return result;
}

size_t max_size (const size_t* array, size_t n)
{
    assert (array != NULL);

    size_t maximum = 0;
    for (size_t i = 0; i < n; ++i)
        maximum = maximum > array[i] ? maximum : array[i];
    return maximum;
}

struct timespec diff_timespec (struct timespec start, struct timespec end)
{
    struct timespec diff;
    if (end.tv_nsec < start.tv_nsec)
    {
        diff.tv_sec = end.tv_sec - start.tv_sec - 1;
        diff.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        diff.tv_sec = end.tv_sec - start.tv_sec;
        diff.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return diff;
}

void print_time (struct timespec time)
{
    printf("%lu.%9lu\n", time.tv_sec, time.tv_nsec);
}
