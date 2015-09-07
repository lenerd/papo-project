#include "math_ext.h"
#include "util.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


float random_value_01 (void)
{
    return random_value_mm (0.0f, 1.0f);
}

float random_value_0m (float max)
{
    return random_value_mm (0.0f, max);
}

float random_value_mm (float min, float max)
{
    return min + (max - min) * (float) rand () / (float) RAND_MAX;
}

float sigmoid(float x){
    return 0.5f * tanhf(x * 0.5f) + 0.5f;
}

#if 0
float sigmoid (float x)
{
    return 1.0f / (1.0f + expf (-x));
}
#endif

float inverse_sigmoid (float y)
{
    return (-logf (1.0f / y - 1.0f));
}

float centered_sigmoid (float x)
{
    return 2.0f * (sigmoid (x) - 0.5f);
}

float* sigmoidize (const float* array, const size_t size)
{
    assert (array != NULL);

    float* result = SAFE_MALLOC (size * sizeof (float));

    for (size_t i = 0; i < size; ++i)
    {
        result[i] = sigmoid (array[i]);
    }

    return result;
}

float* desigmoidize (const float* array, const size_t size)
{
    assert (array != NULL);

    float* result = SAFE_MALLOC (size * sizeof (float));

    for (size_t i = 0; i < size; ++i)
    {
        result[i] = inverse_sigmoid (array[i]);
    }

    return result;
}

float* sigmoidize_inplace (float* array, const size_t size)
{
    assert (array != NULL);

    for (size_t i = 0; i < size; ++i)
    {
        array[i] = sigmoid (array[i]);
    }

    return array;
}

float* desigmoidize_inplace (float* array, const size_t size)
{
    assert (array != NULL);

    for (size_t i = 0; i < size; ++i)
    {
        array[i] = inverse_sigmoid (array[i]);
    }

    return array;
}

uint64_t factorial(uint64_t number)
{
	uint64_t result = 1;

	for(uint64_t i = 2; i <= number; ++i)
		result = result * i;

	return result;
}
