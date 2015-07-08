#include "util.h"

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
