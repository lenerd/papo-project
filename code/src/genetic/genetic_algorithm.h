#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

/**
* \file
* \brief Contains everything required for genetic evolution.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \ingroup genetic
*/


#include <stdint.h>


/**
 * \brief Represents a genome.
 */
typedef struct
{
	/** \brief Count of genes this genome contains. */
	uint32_t genes_count;
	/** \brief Values of all genes. */
	float* genes;
	/** \brief How fit this genome is. */
	float fitness;
} genome_t;

/**
 * \brief Represents a population of genomes.
 */
typedef struct
{

	/** \brief Size of the population. */
	uint32_t size;
	/** \brief Array of pointer towards all genomes of this population. */
	genome_t** individuals;
	/** \brief Array of pointer for buffer-swapping for individuals. */
	genome_t** back_buffer;
	/** \brief The current generation this population is in. */
	uint32_t generation;
	/** \brief Summed up fitnesses of all genomes. */
	float total_fitness;
	/** \brief Average fitness of all genomes of this population. */
	float avg_fitness;
	/** \brief Base fitness added for each individual. Should be as low as possible. */
	float base_fitness;

} population_t;

/** \brief Percentage, how many mutations happen to create new genomes compared to crossovers. I.e. mutation_crossover_ratio = 0.3, so there is a 30% chance that a mutation and a 70% chance that a crossover happens to create a new genome in an advancement of the population. */
float mutation_crossover_ratio;
/** \brief Chance for each gene in a genome to mutate */
float gene_mutation_chance;

/**
* \brief Creates and returns a genome with given genes.
* \pre genes_count > 0
* \pre genes != NULL
*/
genome_t* create_genome(uint32_t genes_count, float* genes);

/**
* \brief Creates and returns a population of genomes based on an array of pointers of precreated genomes.
* \pre population_size > 0
* \pre genomes != NULL
* \pre base_fitness >= 0
*/
population_t* create_population(uint32_t population_size, genome_t** genomes, float base_fitness);

/**
* \brief Creates and returns a mutated version of the given genome.
* \pre gen != NULL
*/
genome_t* mutate_genome(genome_t* gen);

/**
* \brief Creates and returns a crossover child of the given two genomes.
* \pre father != NULL
* \pre mother != NULL
*/
genome_t* crossover_genomes(genome_t* father, genome_t* mother);

/**
* \brief Returns a random genome based on fittnesses 
*        Returns a random genome (individual) of the given population where fitter ones have a higher chance to be returned.
*		 This method supports fitnesses of 0 as long as base_fitness is bigger than 0.
*		 If Fitnesses are always bigger than zero you should use 0 as base_fitness.
* \pre pop != NULL
* \pre base_fitness >= 0
*/
genome_t* select_individual(population_t* pop, float base_fitness);

/**
* \brief Advances a given population one generation with mutation/crossovers defining the genomes of the next generation.
* \pre pop != NULL
*/
void next_generation(population_t* pop);

#endif /* GENETIC_ALGORITHM_H */
