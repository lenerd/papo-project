#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "math_ext.h"


void set_seed() { srand((unsigned int)time(NULL)); }
void set_seed_ifn() {
	if (seed_set == 0){
		set_seed();
		seed_set = 1;
	}
}

float random_value_01()					    { set_seed_ifn(); return (float)rand() / RAND_MAX; }
float random_value_0m(float max)			{ set_seed_ifn(); return max * rand() / RAND_MAX; }
float random_value_mm(float min, float max) { set_seed_ifn(); return min + (max - min) * rand() / RAND_MAX; }

float sigmoid(float x)		   { return 1.0f / (1.0f + (float)exp(x)); }
float inverse_sigmoid(float x) { return (float)(-log(1.0f / x - 1.0f)); }
float centered_sigmoid(float x) { return 2.0f * (sigmoid(x) - 0.5f); }
