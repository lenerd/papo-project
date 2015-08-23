#include <math.h>
#include <stdio.h>

#include "go/board.h"
#include "go/game.h"
#include "go/player.h"
#include "neuralnet/neuralnet.h"
#include "util/math_ext.h"
#include "util/util.h"
#include "training/training.h"
#include "genetic/genetic_algorithm.h"

/*
 * This test should show that genetically trained nets are better players
 * than the nets just trained by the backpropagation. 
 *
 * The Test:
 *
 * -The assumption H_0 is that both nets play with the same quality.
 * -The score won't be used here, just if a game is won.
 * -As a game is like a single experiment, we assume a Binomial distribution
 *  with n=x (number of games) and p = 0.5.
 * - If the hypothesis is rejected with a significance of at least 5%, we can
 *   safely say that the unsupervised learning is successful.
 */

float compute_probability(uint8_t game_count, uint8_t games_won)
{	
	float probability = 0;
	
	/*
 	 * Formula:
 	 *
 	 * P(Games won > games_won | H_0) =
 	 *
 	 * = sum{i=0, games_won}((game_count over i)* (1/2)^i * (1-1/2)^(game_count-i)
 	 * + sum{i=0, (1-games_won)}((game_count over i) * (/2)^i * (1-1/2)^(game_count-i)=
 	 *
 	 * = 2 * (1/ (2 ^ game_count) * sum{i=0, games_won}(games_won over i)
 	 */

	if((game_count - games_won) < games_won)
	{
		for(int i = 0; i < (game_count - games_won); ++i)
			probability += factorial(game_count) / (factorial(i) - factorial(game_count - i)); 
	}
	else
	{
		for(int i = 0; i < games_won; ++i)
			probability += factorial(game_count) / (factorial(i) - factorial(game_count - i));
	}

	probability = probability * 2 * (1/ exp2(game_count));
	return probability;
}

int main()
{
	uint8_t generations = 100000000000000;
	uint8_t nets_per_party = 10;
	uint8_t board_size = 6;
	uint8_t game_count = (nets_per_party * (nets_per_party + 1)) / 2;
	uint8_t games_won = 0;
	const size_t layers = 5;
	const size_t neurons_per_layer[] = {36, 36, 36, 36, 36} ;
	
	//Make nets and backpropagate
	nnet_set_t* trained_nets = nnet_set_create(nets_per_party);	
	nnet_set_t* untrained_nets = nnet_set_create(nets_per_party);

	dataset_t* training_data = dataset_create(50);
	for(int i = 0; i < 50; ++i)
		training_data->data[i] = td_generate_nxn_nxn(board_size);

	for(int i = 0; i < trained_nets->size; ++i)
	{
		trained_nets->nets[i] = nnet_create_random(layers, neurons_per_layer);
		
		for(int j = 0; j < training_data->size; ++j)
		{
			nnet_backpropagate(trained_nets->nets[i], training_data->data[j]->input, training_data->data[j]->expected);
		}
	}

	for(int i = 0; i < untrained_nets->size; ++i)
	{
		untrained_nets->nets[i] = nnet_create_random(layers, neurons_per_layer);
		
		for(int j = 0; j < training_data->size; ++j)
		{
			nnet_backpropagate(untrained_nets->nets[i], training_data->data[j]->input, training_data->data[j]->expected);
		}
	}

	//Do unsupervised learning
	genome_t** genomes = SAFE_MALLOC(nets_per_party * sizeof(genome_t*));
	for(int i = 0; i < nets_per_party; ++i)
		genomes[i] = genome_create(trained_nets->nets[i]->edge_count, &trained_nets->nets[i]->edge_buf, &update_neuralnet, trained_nets->nets[i]);
	
	population_t* population = population_create(nets_per_party, genomes, 0);
	
	for(int i = 0; i < generations; ++i)
	{
		for(int j = 0; j < nets_per_party; ++j)
		{
			player_t* player1 = player_create_net(trained_nets->nets[j]);

			for(int k = j; k < nets_per_party; ++k)
			{
				player_t* player2 = player_create_net(trained_nets->nets[k]);	
			
				game_t* game = game_create(player1, player2, board_size, 5000);
			
				while(!game->finished)
					game_step(game);

				int score = game_score(game);
				population->individuals[j]->fitness += score;
				population->individuals[k]->fitness -= score;
			}
		}

		the_next_generation(population);
	}

	for(int i = 0;  i < nets_per_party; ++i)
		trained_nets->nets[i] = nnet_create_buffer(layers, neurons_per_layer, *genomes[i]->genes);

	population_destroy(population);
	free(genomes);

	//Play and count victories
	for(int j = 0; j < nets_per_party; ++j)
	{
		player_t* trained = player_create_net(trained_nets->nets[j]);

		for(int k = 0; k < nets_per_party; ++k)
		{
			player_t* untrained = player_create_net(untrained_nets->nets[k]);
			
			game_t* game = game_create(trained, untrained, board_size, 5000);
			while(!game->finished)
				game_step(game);

			int score = game_score(game);
			if(score > 0)
				++games_won;

			player_destroy(untrained);
		}
		
		player_destroy(trained);
	}


	//See if the probability is low enough
	float prob = compute_probability(game_count, games_won);
	if(prob < 0.05 && (games_won > (game_count /2)))	
		printf("The training is effective: the hypothesis that the nets are equally strong before and after unsupervised learning was rejected. \n Significance: %0.2f, Number of nets: %d, Number of games: %d, Games won by trained nets: %d \n", prob, (2*nets_per_party), game_count, games_won);
	else
		printf("No such luck! Training seems to have no effect. Try again maybe? \n Significance: %0.3f, Number of nets: %d, Number of games: %d, Games won by trained nets: %d \n", prob, (2*nets_per_party), game_count, games_won);


	nnet_set_destroy(trained_nets);
	nnet_set_destroy(untrained_nets);
	return 1;	
}
