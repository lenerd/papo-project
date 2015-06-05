#include <stdlib.h>
#include "math_extend/math_ext.h"
#include "genetic/genetic_algorithm.h"

float mutation_crossover_ratio = 0.3f;
float gene_mutation_chance = 0.001f;

genome* create_genome(int genes_count, float* genes){

	genome* gen = (genome*)malloc(sizeof(genome));
	gen->genes_count = genes_count;
	gen->genes = genes;
	gen->fitness = 0.0f;
	return gen;

}
population* create_population(int population_size, genome** genomes){

	population* pop = (population*)malloc(sizeof(population));
	pop->size = population_size;
	pop->individuals = genomes;
	
	pop->generation = 0;
	return pop;

}

genome* mutate_genome(genome* gen){

	float* new_genes = (float*)malloc(sizeof(float) * gen->genes_count);

	for (int i = 0; i < gen->genes_count; i++){
		new_genes[i] = gen->genes[i];
		if (random_value_01() < gene_mutation_chance){
			new_genes[i] += inverse_sigmoid(random_value_01());
		}
	}

	return create_genome(gen->genes_count, new_genes);

}
genome* crossover_genomes(genome* father, genome* mother){
	
	if (father->genes_count != mother->genes_count)
		return father;

	int border = (int)random_value_0m((float)father->genes_count);

	float* new_genes = (float*)malloc(sizeof(float) * father->genes_count);

	for (int i = 0; i < father->genes_count; i++){
		new_genes[i] = i > border ? mother->genes[i] : father->genes[i];
	}

	return create_genome(father->genes_count, new_genes);

}

genome* select_individual(population* pop){
	
	float r = random_value_01() * (pop->total_fitness + pop->size);
	float f = 0.0f;
	int i = 0;
	for (; i < pop->size; i++){
		f += pop->individuals[i]->fitness + 1.0f;
		if (f > r) { break; }
	}
	return pop->individuals[i];

}

void next_generation(population* pop){

	for (int i = 0; i < pop->size; i++){
		//pop->individuals[i]
	}

}
