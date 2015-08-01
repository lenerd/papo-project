#include <stdio.h>
#include <stdlib.h>
#include "genetic/genetic_algorithm.h"
#include "newneunet/newneunet.h"
#include "go/game_controller.h"
#include "training/training.h"

struct net_struct
{
	neuralnet_t* net;
	int score;
};

int cmp(struct net_struct* a, struct net_struct* b)
{
	return a->score - b->score;
}

int main (int argc, char** argv)
{
	if(argc < 5 )
		printf("Please start again with the following parameters: \n <number of nets> <number of generations> <board_size> <hidden layers> <neurons per hidden layer> \n optional: <komi> <preexisting population>\n");
	else
	{
		//Setup
		//Read arguments
		int net_count = atoi(argv[1]);
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
		struct net_struct** current_nets = malloc((sizeof(neuralnet_t*) + sizeof(int)) * net_count);

		if(argc >= 7)
		{
			//TODO: Parse command line argument to population
		}
		else
		{
			int* layers = malloc((hidden_layers +2)*sizeof(int));
			layers[0] = board_size * board_size;
			
			for(int a = 1; a <= hidden_layers; ++a)
			{	
				layers[a] = neurons_per_layer;
			}
			layers[hidden_layers+2] = 2;

			for(int i = 0; i < net_count; ++i)
			{
				current_nets[i]->net = create_neural_net_random((hidden_layers+2), layers);
			}
		}
	
		//Train nets
		dataset_t* dataset = generate_training_data("path", board_size, c_black);
		//Backpropagation 	
		for(int j = 0; j < net_count; ++j)
		{
            for (int i = 0; i < dataset->size; ++i)
            {
                backpropagate(current_nets[j]->net, dataset->data[i].input->buffer, dataset->data[i].expected);	
            }
		}	
			
		//For as many times as specified
		for(int i = 0; i < generations; ++i)
		{	

			//Run games
			for(int a = 0; a < net_count; ++a)
			{
				for(int b = a; b < net_count; ++b)
				{
					if(b != a)
					{
						char name[4];
						sprintf(name, "%c%c.sgf", a, b);
						FILE* record = fopen(name, "a+");
						result_t* result = play(board_size, current_nets[a]->net, current_nets[b]->net, komi, record);
						fclose(record);
						current_nets[a]->score += result->score_black;
						current_nets[b]->score += result->score_black;
					}		
				}
			}
			
			for(int a = 0; a < net_count; ++a)
			{
				printf("Score Netz %d: %d \n", a, current_nets[a]->score);
			}
			
			//Sort nets in descending order of scores 
			qsort(current_nets, net_count, sizeof(struct net_struct), cmp);			
			//Save best two nets
			
			//Create population for genetic algorithm
			genome_t** genomes = malloc(sizeof(genome_t) * net_count);
			
			for(int k = 0; k < net_count; ++k)
			{
				//genomes[k] = create_genome();
			}			
		
			population_t* population = create_population(net_count, genomes, 0);
		
			//Make next generation	
			next_generation(population);
			
		}
		//At the end save all nets of the last generation
	}

    return EXIT_SUCCESS;
}


