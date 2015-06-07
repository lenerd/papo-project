#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "math_ext.h"


void set_seed (unsigned int seed)
{
    srand (seed);
    seed_set = true;
}

void set_seed_time (void)
{
    srand ((unsigned int) time (NULL));
    seed_set = true;
}

void set_seed_ifn (void)
{
    if (!seed_set)
    {
        set_seed_time ();
        seed_set = true;
    }
}

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
    set_seed_ifn ();
    return min + (max - min) * (float) rand () / (float) RAND_MAX;
}

float sigmoid (float x)
{
    return 1.0f / (1.0f + expf (-x));
}

float inverse_sigmoid (float y)
{
    return (-logf (1.0f / y - 1.0f));
}

float centered_sigmoid (float x)
{
    return 2.0f * (sigmoid (x) - 0.5f);
}
