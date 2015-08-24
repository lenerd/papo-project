#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

/**
* \file
* \brief Contains everything required for genetic evolution.
* \author Armin Schaare <3schaare@informatik.uni-hamburg.de>
* \ingroup genetic
*/

#include <stddef.h>
#include <stdint.h>

/**
 * \brief Function accepting a void pointer and returning nothing.
 * \param arg Argument of this function.
 */
typedef void (*genes_update_fun)(void* arg);


/**
 * \brief Represents a genome.
 */
typedef struct
{
    /** \brief Count of genes this genome contains. */
    size_t genes_count;
    /** \brief Values of all genes. */
    float** genes;
    /** \brief How fit this genome is. */
    float fitness;
    /** \brief Function to call when *genes is changed. */
    genes_update_fun update_fun;
    /** \brief Argument for update_fun. */
    void* update_arg;
} genome_t;

/**
 * \brief Represents a population of genomes.
 */
typedef struct
{
    /** \brief Size of the population. */
    size_t size;
    /** \brief Array of pointer towards all genomes of this population. */
    genome_t** individuals;
    /** \brief The current generation this population is in. */
    uint64_t generation;
    /** \brief Summed up fitnesses of all genomes. */
    float total_fitness;
    /** \brief Average fitness of all genomes of this population. */
    float avg_fitness;
    /** \brief Base fitness added for each individual. Should be as low as
     * possible. */
    float base_fitness;

} population_t;

/** \brief Percentage, how many mutations happen to create new genomes compared
 * to crossovers. I.e. mutation_crossover_ratio = 0.3, so there is a 30% chance
 * that a mutation and a 70% chance that a crossover happens to create a new
 * genome in an advancement of the population. */
float mutation_crossover_ratio;
/** \brief Chance for each gene in a genome to mutate */
float gene_mutation_chance;

/**
* \brief Creates and returns a genome with given genes.
* \param genes Pointer to the buffer where the genes are stored.
* \param genes_count Sizes of that buffer.
* \param update_fun Function to call, when *genes is modified.
* \param update_arg Argument to pass to above function.
* \pre genes_count > 0
* \pre genes != NULL
* \pre update_fun != NULL
*/
genome_t* genome_create (size_t genes_count, float** genes,
                         genes_update_fun update_fun, void* update_arg);

/**
 * \brief Destroys a genome.
 * \param genome The genome.
 * \pre genome != NULL
 * \post genome is freed.
 */
void genome_destroy (genome_t* genome);

/**
* \brief Creates and returns a population of genomes based on an array of
* pointers of precreated genomes.
* \pre population_size > 0
* \pre genomes != NULL
* \pre base_fitness >= 0
*/
population_t* population_create (size_t size, genome_t** genomes,
                                 float base_fitness);

/**
 * \brief Release all resources of a population including the genomes.
 * \param pop Population to free.
 * \pre pop != NULL
 * \post All resources are released.
 */
void population_destroy (population_t* pop);

/**
 * \brief Mutates a given genome in-place.
 * \param genome Genome to mutate.
 * \pre gen != NULL
 */
void mutate_genome (genome_t* genome);

// /**
// * \brief Creates and returns a crossover child of the given two genomes.
// * \pre father != NULL
// * \pre mother != NULL
// */
// genome_t* crossover_genomes(genome_t* father, genome_t* mother);

/**
 * \brief Returns a random genome based on fittnesses
 *
 * Returns a random genome (individual) of the given population where fitter
 * ones have a higher chance to be returned. This method supports fitnesses of 0
 * as long as base_fitness is bigger than 0. If Fitnesses are always bigger than
 * zero you should use 0 as base_fitness.
 * \param pop
 * \param total_fitness
 * \pre pop != NULL
 * \pre base_fitness >= 0
 * \pre total_fitness = sum of individual fitnesses
 */
genome_t* select_individual (population_t* pop, float total_fitness);

/**
* \brief Advances a given population one generation with mutation/crossovers
* defining the genomes of the next generation.
* \param pop Population to advance.
* \pre pop != NULL
*/
void the_next_generation (population_t* pop);

#endif /* GENETIC_ALGORITHM_H */
