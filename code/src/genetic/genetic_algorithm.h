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
	float total_fitness;
	float avg_fitness;

};

float mutation_crossover_ratio;
float gene_mutation_chance;

struct genome* create_genome(int genes_count, float* genes);
struct population* create_population(int population_size, int genome_size, float** genes);

struct genome* mutate_genome(struct genome* gen);
struct genome* crossover_genomes(struct genome* father, struct genome* mother);

struct genome* select_individual(struct population* pop);
void next_generation(struct population* pop);

#endif /* GENETIC_ALGORITHM_H */