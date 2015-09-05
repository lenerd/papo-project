#include "math_ext.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>

void write_data(){
	//float akku = 0.0f;
	for(uint8_t i = 0; i < 255; ++i){
		float i_1 = (float)i * 0.1f - 12.75f;
		//akku += fabs(sigmoid(i_1) - approx_sigmoid(i_1));
		printf("%f,%f,%f\n", i_1, approx_sigmoid2(i_1), sigmoid(i_1));
	}
}

float calc_error(){
	float akku = 0.0f;
	for(int64_t i = -10; i < 10; ++i){
		
		akku += fabs(sigmoid((float)i) - approx_sigmoid((float)i));
		
	}
	return akku * 0.05;
}

void bench_approx_sigmoid2(){
	
	for(int64_t i = -5000000; i < 5000000; ++i){
		approx_sigmoid2((float)i);
	}

}

void bench_approx_sigmoid(){
	
	for(int64_t i = -5000000; i < 5000000; ++i){
		approx_sigmoid((float)i);
	}

}

void bench_sigmoid(){

	for(int64_t i = -5000000; i < 5000000; ++i){
		sigmoid((float)i);
	}

}

int main(void){
	
	bench_approx_sigmoid2();

}
