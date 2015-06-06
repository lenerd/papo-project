#include <stdlib.h>
#include <stdio.h>
#include "math_extend/math_ext.h"
#include "genetic/genetic_algorithm.h"

float mutation_crossover_ratio = 0.3f;
float gene_mutation_chance = 0.001f;

genome_t* create_genome(uint32_t genes_count, float* genes){

	genome_t* gen = malloc(sizeof(genome_t));
    if (gen == NULL)
    {
        fprintf(stderr, "malloc() failed in file %s at line # %d",
                __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
	gen->genes_count = genes_count;
	gen->genes = genes;
	gen->fitness = 0.0f;
	return gen;

}

population_t* create_population(uint32_t population_size, genome_t** genomes, float base_fitness){

	population_t* pop = malloc(sizeof(population_t));
    if (pop == NULL)
    {
        fprintf(stderr, "malloc() failed in file %s at line # %d",
                __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
	pop->size = population_size;
	pop->individuals = genomes;
	pop->back_buffer = malloc(sizeof(genome_t*) * population_size);
	pop->generation = 0;
	pop->base_fitness = base_fitness;
    pop->total_fitness = 0.0f;
    pop->avg_fitness = 0.0f;
	return pop;

}

genome_t* mutate_genome(genome_t* gen){

	float* new_genes = malloc(sizeof(float) * gen->genes_count);
    if (new_genes == NULL)
    {
        fprintf(stderr, "malloc() failed in file %s at line # %d",
                __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

	for (uint32_t i = 0; i < gen->genes_count; i++){
		new_genes[i] = gen->genes[i];
		if (random_value_01() < gene_mutation_chance){
			new_genes[i] += inverse_sigmoid(random_value_01());
		}
	}

	return create_genome(gen->genes_count, new_genes);

}
genome_t* crossover_genomes(genome_t* father, genome_t* mother){
	
	if (father->genes_count != mother->genes_count)
		return father;

	uint32_t border = (uint32_t) random_value_0m ((float) father->genes_count);

	float* new_genes = malloc(sizeof(float) * father->genes_count);
    if (new_genes == NULL)
    {
        fprintf(stderr, "malloc() failed in file %s at line # %d",
                __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

	for (uint32_t i = 0; i < father->genes_count; ++i)
    {
		new_genes[i] = i > border ? mother->genes[i] : father->genes[i];
	}

	return create_genome(father->genes_count, new_genes);

}

genome_t* select_individual(population_t* pop){

	float r = random_value_01() * (pop->total_fitness + pop->size * pop->base_fitness);
	float f = 0.0f;
	uint32_t i = 0;
	for (; i < pop->size; ++i)
	{
		f += pop->individuals[i]->fitness + pop->base_fitness;
		if (f > r) { break; }
	}
	return pop->individuals[i];

}

void next_generation(population_t* pop){

	for (uint32_t i = 0; i < pop->size; ++i)
    {
		pop->back_buffer[i] = mutate_genome(select_individual(pop));
	}
	for (uint32_t i = 0; i < pop->size; ++i)
	{
		free(pop->individuals[i]);
	}

	genome_t** temp = pop->individuals;
	pop->individuals = pop->back_buffer;
	pop->back_buffer = temp;
	++pop->generation;
}
