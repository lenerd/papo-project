#include <stdio.h>
#include "neuralnet.h"

float* backpropagation(neuralnet_t* net, int board_size, float threshold)
{
	float error = 1000;

	//Get training data for board size

	int data_size;	

	//Do until error is small enough:
	while(error > threshold)
	{		
		//For each data point
		for(int i = 0; i <= data_size; ++i)
		{
			//calculate output
			float output = calculate_output_from_int(net, data[i]);			
			int output_size = board_size * board_size +1

			//calculate difference matrix between output and desired result (can probably be used as error)
			float* wanted = data[i];			
			float* delta = malloc(output_size *sizeof(float));
			
			for(int j = 0; j < output_size; ++j)
			{
				delta[j] = wanted[j]-output[j];	
			}
			
			float* edges = net.edge_buf;

			//For each hidden layer do:
			for(int k = 0; k < net.hidden_layer_count; ++k)
			{
				int neurons = net.neurons_per_hidden_layer;

				//For each output value do:
				for(int l = 0; l < output_size; ++l)
				{
					//Split difference between output and result evenly
					float difference = delta[l] / neurons;

					//Distribute the difference between the edge weights
					for(int m = 0; m < neurons; ++m)
					{
						int position = l + m;
				
						edges[m] += difference;
					}

					
				}
			}
			
			//Replace old net with a new one using the updated weights
			net = create_neural_net_buffer(net.input_count, net.hidden_layer_count, net.neurons_per_hidden_layer, net.output_count, edges);
		}
	}

	return net.edge_buf;
}
