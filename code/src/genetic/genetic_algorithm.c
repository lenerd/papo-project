#include "genetic_algorithm.h"
#include "util/util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util/math_ext.h"
#include "genetic_algorithm.h"

float mutation_crossover_ratio = 1.0f;
float gene_mutation_chance = 0.01f;

genome_t* create_genome (size_t genes_count, float** genes,
                         genes_update_fun update_fun, void* update_arg)
{
    genome_t* genome = SAFE_MALLOC (sizeof (genome_t));

    genome->genes_count = genes_count;
    genome->genes = genes;
    genome->fitness = 0.0f;
    genome->update_fun = update_fun;
    genome->update_arg = update_arg;
    return genome;
}

population_t* create_population (size_t population_size, genome_t** genomes,
                                 float base_fitness)
{
    population_t* pop = SAFE_MALLOC (sizeof (population_t));

    pop->size = population_size;
    pop->individuals = genomes;
    pop->generation = 0;
    pop->base_fitness = base_fitness;
    pop->total_fitness = 0.0f;
    pop->avg_fitness = 0.0f;
    return pop;
}

void mutate_genome (genome_t* genome)
{
    for (size_t i = 0; i < genome->genes_count; i++)
    {
        if (random_value_01 () < gene_mutation_chance)
        {
            (*genome->genes)[i] += inverse_sigmoid (random_value_01 ());
        }
    }
}

// genome_t* crossover_genomes(genome_t* father, genome_t* mother){
//
// 	if (father->genes_count != mother->genes_count)
// 		return father;
//
// 	uint32_t border = (uint32_t) random_value_0m ((float) father->genes_count);
//
// 	float* new_genes = malloc(sizeof(float) * father->genes_count);
//     if (new_genes == NULL)
//     {
//         fprintf(stderr, "malloc() failed in file %s at line # %d\n",
//                 __FILE__, __LINE__);
//         exit(EXIT_FAILURE);
//     }
//
// 	for (uint32_t i = 0; i < father->genes_count; ++i)
//     {
// 		new_genes[i] = i > border ? mother->genes[i] : father->genes[i];
// 	}
//
// 	return create_genome(father->genes_count, new_genes);
//
// }

genome_t* select_individual (population_t* pop)
{
    float r = random_value_01 () *
              (pop->total_fitness + (float) pop->size * pop->base_fitness);
    float f = 0.0f;
    for (size_t i = 0; i < pop->size; ++i)
    {
        f += pop->individuals[i]->fitness + pop->base_fitness;
        if (f > r)
            return pop->individuals[i];
    }
    return pop->individuals[pop->size - 1];
}

void the_next_generation (population_t* pop)
{
    float** new_genes;
    new_genes = SAFE_CALLOC (pop->size, sizeof (float*));

    for (size_t i = 0; i < pop->size; ++i)
    {
        size_t buf_len = pop->individuals[i]->genes_count * sizeof (float);
        new_genes[i] = SAFE_MALLOC (buf_len);
        memcpy (new_genes[i], *(select_individual (pop)->genes), buf_len);
    }

    for (size_t i = 0; i < pop->size; ++i)
    {
        free (*pop->individuals[i]->genes);
        *pop->individuals[i]->genes = new_genes[i];
        if (pop->individuals[i]->update_fun != NULL)
            pop->individuals[i]->update_fun (pop->individuals[i]->update_arg);
        mutate_genome (pop->individuals[i]);
    }

    free (new_genes);
    ++pop->generation;
    pop->total_fitness = 0.0f;
    pop->avg_fitness = 0.0f;
}
