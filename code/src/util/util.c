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

size_t max_size (const size_t* array, size_t n)
{
    assert (array != NULL);

    size_t maximum = 0;
    for (size_t i = 0; i < n; ++i)
        maximum = maximum > array[i] ? maximum : array[i];
    return maximum;
}
