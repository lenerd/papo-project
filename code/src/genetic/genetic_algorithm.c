#include <stdlib.h>
#include "rand_ext.h"
#include "genetic_algorithm.h"

struct genome* create_genome(int genes_count, float* genes){

	struct genome* gen = (struct genome*)malloc(sizeof(struct genome));
	gen->genes_count = genes_count;
	gen->genes = genes;
	gen->fitness = 0.0f;
	return gen;

}
struct population* create_population(int population_size, int genome_size, float** genes){

	struct population* pop = (struct population*)malloc(sizeof(struct population));
	pop->size = population_size;
	pop->individuals = (struct genome**)malloc(sizeof(struct genome*) * population_size);
	
	for (int i = 0; i < population_size; i++){
		pop->individuals[i] = create_genome(genome_size, genes[i]);
	}
	
	pop->generation = 0;
	return pop;

}

void mutate_genome(struct genome* gen){

	for (int i = 0; i < gen->genes_count; i++){
		if (random_value_01() < gene_mutation_chance){
			gen->genes[i] += 
		}
	}

}

int main(int argc, char** argv){



}