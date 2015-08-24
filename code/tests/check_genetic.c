#include <check.h>
#include <stdlib.h>

#include "genetic/genetic_algorithm.h"
#include "util/util.h"

void do_nothing(void* arg)
{
    int* dummy = (int*)arg;
    ++dummy;
}

START_TEST(test_genome_type)
{
	size_t genes_count = 20;
	float genes[20]= {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    float* genes_p = genes;

	genome_t* genome = genome_create(genes_count, &genes_p, &do_nothing, NULL);

	ck_assert(genome != NULL);
	ck_assert(genome->genes_count == genes_count);
	ck_assert(!(genome->fitness < 0.0f) && !(genome->fitness > 0.0f));
	ck_assert(!((*genome->genes)[2] < genes[2]) && !((*genome->genes)[2] > genes[2]));
	ck_assert(!((*genome->genes)[3] < genes[3]) && !((*genome->genes)[3] > genes[3]));

	genome_destroy(genome);	
}
END_TEST

START_TEST(test_population_type)
{
	size_t size = 9;
	float genes[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    float* genes_p = genes;

	genome_t** genomes = SAFE_MALLOC(size*sizeof(genome_t*));
	for(size_t i = 0; i < size; ++i)
		genomes[i] = genome_create(size, &genes_p, &do_nothing, NULL);

	ck_assert(genomes != NULL);

	population_t* pop = population_create(size, genomes, 1);

	ck_assert(pop != NULL);
	ck_assert(pop->size == size);
	ck_assert(!(pop->base_fitness < 1.0f) && !(pop->base_fitness > 1.0f));
	ck_assert(!(pop->avg_fitness < 1.0f) && !(pop->avg_fitness > 1.0f));
	ck_assert(pop->total_fitness = (float)size);
	ck_assert(pop->individuals[2]->genes[3] == genomes[2]->genes[3]);
	ck_assert(pop->individuals[2]->genes[4] == genomes[2]->genes[4]);

	population_destroy(pop);
	free(genomes);

}
END_TEST

START_TEST(test_mutation)
{
	size_t genes_count = 2;
	float genes[2] = {2, 3};
    float* genes_p = genes;

	genome_t* genome = genome_create(genes_count, &genes_p, &do_nothing, NULL);

	for(int i = 0; i < 1000; ++i)
		mutate_genome(genome);

	ck_assert(!((*genome->genes)[0] < genes[0]) && !((*genome->genes)[0] > genes[0]));
	ck_assert(!((*genome->genes)[1] < genes[1]) && !((*genome->genes)[1] > genes[1]));

	genome_destroy(genome);
}
END_TEST

START_TEST(test_selection)
{
	size_t size = 9;
	float genes1[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	float genes2[9] = {11, 12, 13, 14, 15, 16, 17, 18, 19};
	float genes3[9] = {21, 22, 23, 24, 25, 26, 27, 28, 29};	
    float* genes1_p = genes1;
    float* genes2_p = genes2;
    float* genes3_p = genes3;

	genome_t** genomes = SAFE_MALLOC(size*sizeof(genome_t*));
	genomes[0] = genome_create(size, &genes1_p, &do_nothing, NULL);
	genomes[1] = genome_create(size, &genes2_p, &do_nothing, NULL);
	genomes[2] = genome_create(size, &genes3_p, &do_nothing, NULL);

	ck_assert(genomes != NULL);

	population_t* pop = population_create(size, genomes, 1);

	pop->individuals[0]->fitness = 5;
	pop->individuals[1]->fitness = 50;
	pop->individuals[2]->fitness = 500;

	int count1 = 0;
	int count2 = 0;
	int count3 = 0;

	for(int i = 0; i < 1000; ++i)
	{
		genome_t* ind = select_individual(pop);
		
		if(*ind->genes == genes1)
			++count1;
		else if(*ind->genes == genes2)
			++count2;
		else
			++count3;
	}

	ck_assert(count1 <= count2);
	ck_assert(count2 <= count3);

	population_destroy(pop);
	free(genomes);

}
END_TEST

START_TEST(test_next_generation)
{
}
END_TEST

Suite* check_genetic_algorithm (void)
{
	Suite* s;
	TCase* tc_core;
	s = suite_create ("Genetic Algorithm");

	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_genome_type);
	tcase_add_test(tc_core, test_population_type);
	tcase_add_test(tc_core, test_mutation);
	tcase_add_test(tc_core, test_selection);
	tcase_add_test(tc_core, test_next_generation);

	suite_add_tcase (s, tc_core);
	
	return s;
}

int main (void)
{
	int n;
	SRunner* sr;

	sr = srunner_create (check_genetic_algorithm());
	
    srunner_set_fork_status (sr, CK_NOFORK);
	srunner_run_all(sr, CK_VERBOSE);

	n = srunner_ntests_failed (sr);
	srunner_free(sr);
	return(n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
