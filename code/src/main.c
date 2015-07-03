#include <stdio.h>
#include <stdlib.h>
#include "neuralnet/neuralnet.h"
#include "go/game_controller.h"

int main (int argc, char** argv)
{
	if(argc < 5 )
		printf("Please start again with the following parameters: \n <number of nets> <number of generations> <board_size> <hidden layers> <neurons per hidden layer> \n optional: <preexisting population> <komi>\n");
	else
	{
		//Setup
		//Read arguments
		int nets = atoi(argv[1]);
		int generations = atoi(argv[2]);
		int board_size = atoi(argv[3]);
		int hidden_layers = atoi(argv[4]);
		int neurons_per_layer = atoi(argv[5]);
		float komi;

		//Read optional komi argument or set to standard value
		if(argc == 7)
			komi = atof(argv[4]);
		else
			komi = 4.5;
					
		//Create as many nets as specified or take the given population
		neuralnet_t** population = malloc(sizeof(neuralnet_t*) * nets);
		int input_count = board_size * board_size;
		int output_count = input_count +1;

		if(argc >= 7)
		{
			//TODO: Parse command line argument to population
		}
		else
		{
			for(int i = 0; i < nets; ++i)
			{
				population[i] = create_neural_net_random(input_count, hidden_layers, neurons_per_layer, output_count);
			}
		}
	
		//Train nets
				
		//For as many times as specified
		for(int i = 0; i < generations; ++i)
		{	
			float* scores = malloc(sizeof(int) * nets);

			//Run games
			for(int a = 0; a < nets; ++a)
			{
				for(int b = a; b < nets; ++b)
				{
					if(b != a)
					{
						char name[4] = {(char) a, (char) b, '.c', '\0'};
						FILE* record = fopen(name, "a+");
						result_t result = play(board_size, population[a], population[b], komi, record);
						fclose(record);
						scores[a] += result.score_black;
						scores[b] += result.score_black;
					}		
				}
			}

			//Save best two nets
			//Create next generation
		}
	
		//At the end save all nets
	}

    printf("Hello World\n");

    return EXIT_SUCCESS;
}
