#include <stdio.h>
#include <stdlib.h>
#include "neuralnet/neuralnet.h"

//Testing and Debugging
int tmain(int argc, char** argv){
	/*
	//1 input, 1 hidden layer, 1 neurons per hidden layer, 1 output.
	neuralnet_t* n1 = allocate_neural_net(1, 1, 1, 1);
	float* edges = (float*)malloc(sizeof(float) * n1->edges_count);
	edges[0] = 0.0f;
	edges[1] = 1.0f;
	edges[2] = 1.0f;
	initialize_neural_net_buffer(n1, edges);
	free(edges);

	print_neural_net(n1);

	float n1_in[] = { 1.0f };
	float n1_out[1];

	calculate_output(n1, n1_in, n1_out);
	printf("Input: %f\nOutput: %f\n\n", n1_in[0], n1_out[0]);

	//3 input, 4 hidden layer, 6 neurons per hidden layer, 2 output.
	neuralnet_t* n2 = create_neural_net_random(3, 4, 6, 2);

	print_neural_net(n2);

	float n2_in[] = { 0.0f, 0.0f, 0.0f };
	float n2_out[2];

	calculate_output(n2, n2_in, n2_out);
	printf("Input: %f, %f, %f\nOutput: %f, %f\n\n", n2_in[0], n2_in[1], n2_in[2], n2_out[0], n2_out[1]);

	//1 input, 1 hidden layer, 1 neurons per hidden layer, 1 output.
	neuralnet_t* n3 = create_neural_net_random(1, 1, 1, 1);

	print_neural_net(n3);

	float n3_in[] = { 0.0f };
	float n3_out[1];

	calculate_output(n3, n3_in, n3_out);
	printf("Input: %f\nOutput: %f\n\n", n3_in[0], n3_out[0]);
	*/
	getchar();

	return 0;
}
