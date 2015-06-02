#ifndef MATH_EXT_H
#define MATH_EXT_H

int seed_set;									//Represents wether the seed for random values have been set, yet (1 for true and 0 for false).

void set_seed();								//Sets the seed for random number generation to the time.
void set_seed_ifn();							//Runs set_seed, if the seed hasn't been set already.

float random_value_01();						//Returns a random float between 0 and 1.
float random_value_0m(float max);				//Returns a random float between 0 and max.
float random_value_mm(float min, float max);	//Returns a random float between min and max.

float sigmoid(float x);							//Mathematical sigmoid function. Output values are between 0.0 and 1.0, exclusive. Cuts the y-Axis at 0.5.
float inverse_sigmoid(float x);					//Inverse of the sigmoid function (x has to be between 0.0 and 1.0, exclusive)
float centered_sigmoid(float x);				//Centered version of the Sigmoid function. Output values range from -1.0 to 1.0 (exclusive) with the center of the function at (0|0).

#endif /* MATH_EXT_H */