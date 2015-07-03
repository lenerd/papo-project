#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "neuralnet/neuralnet.h"
#include "training.h"

float* backpropagation(neuralnet_t* net, int board_size, float threshold)
{
	float error = 1000;

	//Get training data for board size
	int** data = generate_data(board_size);
	
	int data_size = data [0];	

	//Do until error is small enough:
	while(error > threshold)
	{		
		//For each data point
		for(int i = 1; i <= data_size; ++i)
		{
			//calculate output
			float* output = calculate_output_from_int(net, data[i]);			
			int output_size = board_size * board_size +1;

			//calculate difference matrix between output and desired result (can probably be used as error)
			float* wanted = data[i];			
			float* delta = malloc(output_size *sizeof(float));
			
			for(int j = 0; j < output_size; ++j)
			{
				delta[j] = wanted[j]-output[j];	
			}
			
			float* edges = net->edge_buf;

			//For each hidden layer do:
			for(int k = 0; k < net->hidden_layer_count; ++k)
			{
				int neurons = net->neurons_per_hidden_layer;

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
			net = create_neural_net_buffer(net->input_count, net->hidden_layer_count, net->neurons_per_hidden_layer, net->output_count, edges);
		}
	}	
	
	return net->edge_buf;
}

int** generate_data(int size)
{
	//Sees how many files there are in this directory
	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	char path[100];
	sprintf(path, "%d", size);
	dirp = opendir(path); 
	if(dirp != NULL)
	{
		while ((entry = readdir(dirp)) != NULL) {
		         file_count++;
		}
		closedir(dirp);
	}
	else
	{
		perror("Error while opening directory.\n");
     		exit(EXIT_FAILURE);
	}

	//Allocates pointer
	int** data = calloc(file_count * size * size, sizeof(int**));
	data[0] = file_count;

	//Opens all files and saves their contents in data
	char fn[100];
	for(int i = 1; i <= file_count; ++i)
	{
		sprintf(fn,"%d/game[%d].bmp", size, i);
		FILE* fp=fopen(fn, "rb");
		data[i]=read_file(fp, size);
		fclose(fp);
	}
}

int* read_file(FILE* fp, int size)
{
	int x, y, position;
	int* game = calloc(size*size, sizeof(int*));
	char c;

	while(c = (fgetc(fp)) != EOF)
	{
		if(c == 'B')
		{
			c = fgetc(fp);
			c = fgetc(fp);
			x = ((int) c) - 97;
			c = fgetc(fp);
			y = ((int) c) - 97;
			position = x*size + y;
			game[position] = 1;
		}
		else if(c == 'W')
		{
			c = fgetc(fp);
			c = fgetc(fp);
			x = ((int) c) - 97;
			c = fgetc(fp);
			y = ((int) c) - 97;
			position = x*size + y;
			game[position] = 2;
		}
	}
	
	return game;
}
