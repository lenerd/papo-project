#include "../src/newneunet/newneunet.h"
#include "../src/util/util.h"
#include "../src/util/math_ext.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/*
 * Benchmark for the backpropagation
 *
 * Measures time: input/time
 *
 * Input size stays the same:
 * 	 int 5 x 5
 *
 * Net structure for test:
 * 	5 5 5 5 5
 */

int main()
{
	int input_size = 5;
	int runs = 1000000;

	struct timespec start, end, total;
	
	int layers[] = {5, 5, 5, 5, 5};
	neuralnet_t* net = create_neural_net_random(5, layers);

	float* bench_input = SAFE_MALLOC(input_size *  sizeof(float));
	float* bench_expected = SAFE_MALLOC(input_size * sizeof(float));

	for(int i = 0; i < runs; ++i)
	{
		for(int j = 0; j < input_size; ++j)
		{
			bench_input[j] = random_value_0m(10000);
			bench_expected[j]= random_value_0m(10000);
		}
		
		clock_gettime(CLOCK_REALTIME, &start);

		backpropagate(net, bench_input, bench_expected);
		
		clock_gettime(CLOCK_REALTIME, &end);
		
		struct timespec diff = diff_timespec(start, end);
		total.tv_sec += diff.tv_sec;
		total.tv_nsec += diff.tv_nsec;
		
	} 
	
	struct timespec per_run;
	per_run.tv_sec = total.tv_sec / runs;
	per_run.tv_nsec = total.tv_nsec / runs;

	printf("In total %lu s and %lu ns for %d backpropagations \n That's %lu sand %lu ns per run\n", total.tv_sec, total.tv_nsec, runs, per_run.tv_sec, per_run.tv_nsec);

	return EXIT_SUCCESS;
}
