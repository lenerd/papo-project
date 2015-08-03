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

	clock_t start, end;
	float total;
	
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
		
		start = clock();

		backpropagate(net, bench_input, bench_expected);
		
		end = clock();

		total += (float) end - start;
		
	} 

	float per_run = (total / CLOCKS_PER_SEC) / runs;

	printf("In total %4.4f s for %d backpropagations \n That's %4.4f s per run\n", total, runs, per_run);

	return EXIT_SUCCESS;
}
