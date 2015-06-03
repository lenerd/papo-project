#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

/**
* \file
* \brief Contains everything required for genetic evolution.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \ingroup go
*/

/**
* \brief Represents a genome.
*/
struct genome{

	/** \brief Count of genes this genome contains. */
	int genes_count;
	/** \brief Values of all genes. */
	float* genes;
	/** \brief How fit this genome is. */
	float fitness;

};
typedef struct genome genome;

/**
* \brief Represents a population of genomes.
*/
struct population{

	/** \brief Size of the population. */
	int size;
	/** \brief Array of pointer towards all genomes of this population. */
	struct genome** individuals;
	/** \brief The current generation this population is in. */
	int generation;
	/** \brief Summed up fitnesses of all genomes. */
	float total_fitness;
	/** \brief Average fitness of all genomes of this population. */
	float avg_fitness;

};
typedef struct population population;

/** \brief Percentage, how many mutations happen to create new genomes compared to crossovers. I.e. mutation_crossover_ratio = 0.3, so there is a 30% chance that a mutation and a 70% chance that a crossover happens to create a new genome in an advancement of the population. */
float mutation_crossover_ratio;
/** \brief Chance for each gene in a genome to mutate */
float gene_mutation_chance;

/**
* \brief Creates and returns a genome with given genes.
* \pre genes_count > 0
* \pre genes != NULL
*/
genome* create_genome(int genes_count, float* genes);

/**
* \brief Creates and returns a population of genomes based on an array of pointers of precreated genomes.
* \pre population_size > 0
* \pre genomes != NULL
*/
population* create_population(int population_size, genome** genomes);

/**
* \brief Creates and returns a mutated version of the given genome.
* \pre gen != NULL
*/
genome* mutate_genome(genome* gen);

/**
* \brief Creates and returns a crossover child of the given two genomes.
* \pre father != NULL
* \pre mother != NULL
*/
genome* crossover_genomes(genome* father, genome* mother);

/**
* \brief Returns an genome (individual) of the given population where fitter ones have a higher chance to be returned.
* \pre pop != NULL
*/
genome* select_individual(population* pop);

/**
* \brief Advances a given population one generation with mutation/crossovers defining the genomes of the next generation.
* \pre pop != NULL
*/
void next_generation(population* pop);

#endif /* GENETIC_ALGORITHM_H */