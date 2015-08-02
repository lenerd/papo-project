#include <stdio.h>
#include <stdlib.h>
#include "newneunet/newneunet.h"
#include "go/game_controller.h"
#include "training/training.h"

int main (int argc, char** argv)
{
	if(argc < 5 )
		printf("Please start again with the following parameters: \n <number of nets> <number of generations> <board_size> <hidden layers> <neurons per hidden layer> \n optional: <komi> <preexisting population>\n");
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
				population[i] = create_neural_net_random(0, NULL);
			}
		}
	
		//Train nets
		struct dataset* data = generate_data(board_size, c_black);
		//TODO Backpropagation 	
		
			
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
						char name[4];
						sprintf(name, "%c%c.sgf", a, b);
						FILE* record = fopen(name, "a+");
						result_t* result = play(board_size, population[a], population[b], komi, record);
						fclose(record);
						scores[a] += result->score_black;
						scores[b] += result->score_black;
					}		
				}
			}
			
			for(int a = 0; a < nets; ++a)
			{
				printf("Score Netz %d: %d \n", a, scores[a]);
			}

			//Save best two nets
			//Create next generation
		}

		//At the end save all nets of the last generation
	}

    return EXIT_SUCCESS;
}
