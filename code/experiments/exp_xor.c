#include "neuralnet/neuralnet.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (void)
{
    size_t size[] = {2, 2, 1};

    float inputs[4][2] = {
        {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};
    float targets[4][1] = {{0.0f}, {1.0f}, {1.0f}, {0.0f}};

    float* output;
    neuralnet_t* net;

    if (access ("./xor.net", R_OK) != -1)
    {
        net = nnet_from_file ("./xor.net", false);
        for (int i = 0; i < 4; ++i)
        {
            output = nnet_calculate_output (net, inputs[i]);
            printf ("%f ^ %f = %f\n", (double) inputs[i][0],
                    (double) inputs[i][1], (double) output[0]);
            free (output);
        }
    }
    else
    {
        net = nnet_create_random (3, size);

        for (int i = 0; i < 4; ++i)
        {
            output = nnet_calculate_output (net, inputs[i]);
            printf ("%f ^ %f = %f\n", (double) inputs[i][0],
                    (double) inputs[i][1], (double) output[0]);
            free (output);
        }
        putchar ('\n');

        for (int n = 0; n < 100000; ++n)
            for (int i = 0; i < 4; ++i)
                nnet_backpropagate (net, inputs[i], targets[i]);

        for (int i = 0; i < 4; ++i)
        {
            output = nnet_calculate_output (net, inputs[i]);
            printf ("%f ^ %f = %f\n", (double) inputs[i][0],
                    (double) inputs[i][1], (double) output[0]);
            free (output);
        }
        nnet_to_file (net, "./xor.net", false);
    }

    nnet_destroy (net);

    return 0;
}
