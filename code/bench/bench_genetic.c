#define _POSIX_C_SOURCE 199309L

#include "genetic/genetic_algorithm.h"
#include "util/util.h"
#include "util/math_ext.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Benchmark for the genetic algorithm.
 *
 * Measures the time needed for x generations
 *
 * Number of individuals:
 * 	10
 * Genome size to be tested:
 *	5 genes
 */

int main()
{
	size_t individual_count = 10;
	size_t gene_count = 5;

	population_t* pop = SAFE_MALLOC(individual_count * gene_count * sizeof(population_t*)); 
	genome_t** genomes = SAFE_MALLOC(individual_count * gene_count * sizeof(genome_t*));

	for(size_t i = 0; i < individual_count; ++i)
	{
		float* genes = SAFE_MALLOC(gene_count * sizeof(float));
		
		for(size_t j = 0; j < gene_count; ++j)
		{
			genes[j] = random_value_0m(10000);
		}
		
		genome_t* genome = genome_create(gene_count, &genes, NULL, NULL);
		genomes[i] =  genome;

	}

	pop = population_create(individual_count, genomes, 0);

	int generations = 1000000;
	struct timespec start, end, total;
    total.tv_sec = 0;
    total.tv_nsec = 0;

	clock_gettime(CLOCK_REALTIME, &start);

	for(int k = 0; k < generations; ++k)
	{
        ;
		// next_generation;
	}

	clock_gettime(CLOCK_REALTIME, &end);

	total = diff_timespec(start, end);

	struct timespec per_gen;
	per_gen.tv_sec = total.tv_sec / generations;
	per_gen.tv_nsec = total.tv_nsec /generations;

	printf("The total time was %lu s %lu ns for %d generations, \n That's %lu s %lu ns per generation", total.tv_sec, total.tv_nsec, generations, per_gen.tv_sec, per_gen.tv_nsec);

	return EXIT_SUCCESS;
}
