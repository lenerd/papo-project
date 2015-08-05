#include "neuralnet/neuralnet.h"
#include "util/util.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


int main (void)
{
#if 1
    {
    struct timespec start, end, diff;

    size_t* size = SAFE_MALLOC(100 * sizeof(size_t));
    for (size_t i = 0; i < 100; ++i)
        size[i] = 100;

    neuralnet_t* net = nnet_create_random (100, size);

    float* input;
    float* output;
    input = SAFE_MALLOC(100 * sizeof(float));
    for (int i = 0; i < 100; ++i)
        input[i] = random_value_mm(0,1);

    timespec_get(&start, TIME_UTC);
    for (int i = 0; i < 1000; ++i)
    {
        output = nnet_calculate_output(net, input);
        free(input);
        input = output;
    }
    timespec_get(&end, TIME_UTC);

    free(output);
    free(size);

    nnet_destroy(net);

    diff = diff_timespec(start, end);
    print_time(diff);
    }
#endif

    return EXIT_SUCCESS;
}
