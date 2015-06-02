#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

struct genome{

	int genes_count;
	float* genes;
	
	float fitness;

};
struct population{

	int size;
	struct genome** individuals;

	int generation;

};

float gene_mutation_chance;

struct genome* create_genome(int genes_count, float* genes);
struct population* create_population(int population_size, int genome_size, float** genes);

void mutate_genome(struct genome* gen);
void crossover_genomes(struct genome* father, struct genome* mother);

#endif /* GENETIC_ALGORITHM_H */