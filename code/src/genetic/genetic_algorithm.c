#include "genetic_algorithm.h"
#include "util/util.h"
#include "util/math_ext.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float mutation_crossover_ratio = 1.0f;

genome_t* genome_create (size_t genes_count, float** genes,
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

void genome_destroy (genome_t* genome)
{
    assert (genome != NULL);

    free (genome);
}

population_t* population_create (size_t size, genome_t** genomes,
                                 float base_fitness)
{
    population_t* pop = SAFE_MALLOC (sizeof (population_t));

    pop->size = size;
    pop->individuals = genomes;
    pop->generation = 0;
    pop->base_fitness = base_fitness;
    pop->mutation_chance = 0.01f;

    // FIXME: remove ?
    pop->total_fitness = 0.0f;
    pop->avg_fitness = 0.0f;

    return pop;
}

void population_destroy (population_t* pop)
{
    assert (pop != NULL);

    for (size_t i = 0; i < pop->size; ++i)
        if (pop->individuals[i] != NULL)
            genome_destroy (pop->individuals[i]);
    free (pop);
}

void mutate_genome (genome_t* genome, float mutation_chance)
{
    for (size_t i = 0; i < genome->genes_count; i++)
    {
        if (random_value_01 () < mutation_chance)
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

genome_t* select_individual (population_t* pop, float total_fitness)
{
    float r = random_value_01 () *
              (total_fitness + (float) pop->size * pop->base_fitness);
    float f = 0.0f;
    for (size_t i = 0; i < pop->size; ++i)
    {
        f += pop->individuals[i]->fitness + pop->base_fitness;
        if (f > r)
            return pop->individuals[i];
    }
    return pop->individuals[pop->size - 1];
}

float total_fitness (const population_t* pop)
{
    assert (pop != NULL);

    float sum = 0.0f;
    for (size_t i = 0; i < pop->size; ++i)
        sum += pop->individuals[i]->fitness;
    return sum;
}

void the_next_generation (population_t* pop)
{
    float** new_genes;
    new_genes = SAFE_CALLOC (pop->size, sizeof (float*));

    float total = total_fitness (pop);

    for (size_t i = 0; i < pop->size; ++i)
    {
        size_t buf_len = pop->individuals[i]->genes_count * sizeof (float);
        new_genes[i] = SAFE_MALLOC (buf_len);
        memcpy (new_genes[i], *(select_individual (pop, total)->genes), buf_len);
    }

    for (size_t i = 0; i < pop->size; ++i)
    {
        free (*pop->individuals[i]->genes);
        *pop->individuals[i]->genes = new_genes[i];
        if (pop->individuals[i]->update_fun != NULL)
            pop->individuals[i]->update_fun (pop->individuals[i]->update_arg);
        mutate_genome (pop->individuals[i], pop->mutation_chance);
    }

    free (new_genes);
    ++pop->generation;
    pop->total_fitness = 0.0f;
    pop->avg_fitness = 0.0f;
}
