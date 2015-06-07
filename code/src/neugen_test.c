#include <stdio.h>
#include <stdlib.h>
#include "neuralnet/neuralnet.h"
#include "genetic/genetic_algorithm.h"

int main(int argc, char** argv){

	uint32_t population_size = 10;

	uint32_t input_count = 2;
	uint32_t output_count = 1;
	uint32_t hidden_layer_count = 1;
	uint32_t neurons_per_hidden_layer = 1;

	uint32_t generations = 10;
	uint32_t tests_per_genome = 100;

	neuralnet_t** nnets = malloc(population_size * sizeof(neuralnet_t*));
	if (nnets == NULL)
	{
		fprintf(stderr, "malloc() failed in file %s at line # %d",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	genome_t** genomes = malloc(population_size * sizeof(genome_t*));
	if (genomes == NULL)
	{
		fprintf(stderr, "malloc() failed in file %s at line # %d",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	for (uint32_t i = 0; i < population_size; i++){
		nnets[i] = create_neural_net_random(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);
		genomes[i] = create_genome(nnets[i]->edges_count, &nnets[i]->edges);
	}

	population_t* pop = create_population(population_size, genomes, 0.0f);

	for (uint32_t i = 0; i < generations; ++i)
	{	
		for (uint32_t j = 0; j < pop->size; ++j)
		{
			float sum_err = 0.0f;
			for (uint32_t k = 0; k < tests_per_genome; ++k)
			{
				float ins[] = { (float)k, (float)k };
				float* out = calculate_output(nnets[j], ins);
				float err = out[0] - 2 * k;
				float sqr_err = err * err;
				sum_err += sqr_err;
			}
			float fit = 1.0f / sum_err;
			pop->individuals[j]->fitness = fit;
			pop->total_fitness += fit;
		}
		pop->avg_fitness = pop->total_fitness / pop->size;
		printf("Generation: %d\n\tTotal Fitness: %f\n\tAverage Fitness: %f\n\n", i, pop->total_fitness, pop->avg_fitness);
		next_generation(pop);
	}

    for (uint32_t i = 0; i < pop->size; ++i)
        destroy_neural_net(nnets[i]);
    free(nnets);
    for (uint32_t i = 0; i < pop->size; ++i)
        free(pop->individuals[i]);
    free(pop->individuals);
    free(pop);


	return 0;

}