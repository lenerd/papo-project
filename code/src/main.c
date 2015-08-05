#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "genetic/genetic_algorithm.h"
#include "neuralnet/neuralnet.h"
#include "go/game_controller.h"
#include "training/training.h"

struct net_struct
{
	neuralnet_t* net;
	int score;
};

struct net_struct* make_net_struct()
{
	struct net_struct* made = SAFE_MALLOC(sizeof(struct net_struct));
	made->score = 0;
	made->net = SAFE_MALLOC(sizeof(neuralnet_t));
	return made;
}

int cmp(struct net_struct* a, struct net_struct* b)
{
	return a->score - b->score;
}

int main (int argc, char** argv)
{
	//For now we don't need the neurons_per_hidden_layer argument 
	//because the size of the board will determine the number of input or output neurons.
	//For the intermediate ones we can come up with a solution later.
	if(argc < 4 )
		printf("Please start again with the following parameters: \n <number of nets> <number of generations> <board_size> <hidden layers> \n optional: <komi> <preexisting population>\n");
	else
	{
		//Setup
		//Read arguments
		int net_count = atoi(argv[1]);
		int generations = atoi(argv[2]);
		int board_size = atoi(argv[3]);
		int hidden_layers = atoi(argv[4]);
		size_t neurons_per_layer = board_size * board_size;
		float komi;

		//Read optional komi argument or set to standard value
		if(argc == 6)
			komi = atof(argv[4]);
		else
			komi = 4.5;
					
		//Create as many nets as specified or take the given population
		struct net_struct** current_nets = SAFE_MALLOC(net_count * sizeof(struct net_struct));
		for(int a = 0; a < net_count; ++a)
		{
			current_nets[a] = make_net_struct();
		}

		if(argc >= 6)
		{
			//TODO: Parse command line argument to population
		}
		else
		{
			size_t* layers = SAFE_MALLOC((hidden_layers+2)*sizeof(size_t)); //+2 because input and output layers ar not hidden ones
			for(int a = 0; a < hidden_layers + 2; ++a) //see line 65 comment
			{	
				layers[a] = neurons_per_layer;
			}

			for(int i = 0; i < net_count; ++i)
			{
				current_nets[i]->net = create_neural_net_random(hidden_layers + 2, layers);  //see line 65 comment
			}
		}
	
		//Train nets
		//Generate data 
		char path[100];
		sprintf(path, "../../src/training/data/%d", board_size);
		dataset_t* dataset = generate_training_data(path, board_size, c_black);

		//Backpropagation 	
		for(int j = 0; j < net_count; ++j)
		{
            printf("Dataset-size: %d", dataset->size);
            for (int i = 0; i < dataset->size; ++i)
    	    {
    	    	float* ins = uint8_array_to_float(dataset->data[i].input->buffer, current_nets[j]->net->neurons_per_layer[0]);
    	    	float* outs = int_array_to_float(dataset->data[i].expected, current_nets[j]->net->neurons_per_layer[current_nets[j]->net->layer_count-1]);
            	backpropagate(current_nets[j]->net, ins, outs);	
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
						sprintf(name, "./kifu/%c%c.sgf", a, b);
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
			sprintf(path, "./nets/%d1", i);
			neural_net_to_file(current_nets[0]->net, path, false);
		
			sprintf(path, "./nets/%d2", i);
			neural_net_to_file(current_nets[1]->net, path, false); 		

			//TODO: Convert edge weights to genomes
			/*	
			//Create population for genetic algorithm
			genome_t** genomes = malloc(sizeof(genome_t) * net_count);
			
			for(int k = 0; k < net_count; ++k)
			{
				//genomes[k] = create_genome();
			}			
		
			population_t* population = create_population(net_count, genomes, 0);
		
			//Make next generation	
			next_generation(population);
			*/
			
		}
		//At the end save all nets of the last generation
		for(int l = 0; l < net_count; ++l)
		{
			sprintf(path, "./nets/%d%d", generations, l);	
			neural_net_to_file(current_nets[l]->net, path, false);
		}
	}

    return EXIT_SUCCESS;
}


