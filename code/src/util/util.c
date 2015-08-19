#include "util.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
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


void* safe_realloc (void* ptr, size_t size, const char* file,
                    unsigned long line)
{
    assert (ptr != NULL);
    assert (size != 0);

    void* p = realloc (ptr, size);
    if (p == NULL)
    {
        fprintf (stderr, "[%s:%lu] realloc failed (%lu bytes)\n", file, line,
                 size);
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

struct timespec sum_timespec (struct timespec t1, struct timespec t2)
{
    struct timespec sum;
    sum.tv_nsec = t1.tv_nsec + t2.tv_nsec;
    sum.tv_sec = sum.tv_nsec / 1000000000;
    sum.tv_nsec %= 1000000000;
    sum.tv_sec += t1.tv_sec + t2.tv_sec;
    return sum;
}

struct timespec div_timespec (struct timespec t, uint64_t d)
{
    t.tv_nsec /= d;
    t.tv_nsec += (t.tv_sec % d) * 1000000000 / d;
    t.tv_sec /= d;
    return t;
}

double timespec_to_double (struct timespec t)
{
    double d = t.tv_nsec;
    d /= 1000000000;
    d += t.tv_sec;
    return d;
}

uint64_t timespec_to_uint64 (struct timespec t)
{
    uint64_t u = t.tv_sec;
    u *= 1000000000;
    u += t.tv_nsec;
    return u;
}

void print_time (struct timespec time)
{
    printf("%lu.%09lu\n", time.tv_sec, time.tv_nsec);
}
