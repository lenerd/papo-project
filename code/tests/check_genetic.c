#include <check.h>
#include <stdlib.h>

#include "genetic/genetic_algorithm.h"
#include "util/util.h"

void do_nothing(void* arg)
{
}

START_TEST(test_genome_type)
{
	int genes_count = 5;
	float genes[5][4]= {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}, {17, 18, 19, 20}};

	genome_t* genome = genome_create(genes_count, genes, *do_nothing, 5);

	ck_assert(genome != NULL);
	ck_assert(genome->genes_count == genes_count);
	ck_assert(genome->fitness == 0.0f);
	ck_assert(genome->genes[2][3] == genes[2][3]);
	ck_assert(genome->genes[2][2] == genes[2][2]);

	genome_destroy(genome);	
}
END_TEST

START_TEST(test_population_type)
{
	int size = 7;
	float genes[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

	genome_t** genomes = SAFE_MALLOC(size*sizeof(genome_t*));
	for(int i = 0; i < size; ++i)
		genomes[i] = genome_create(5, genes, *do_nothing, 5);

	ck_assert(genomes != NULL);

	population_t* pop = population_create(size, genomes, 1);

	ck_assert(pop != NULL);
	ck_assert(pop->size == size);
	ck_assert(pop->base_fitness == 1);
	ck_assert(pop->avg_fitness == 1);
	ck_assert(pop->total_fitness = size);
	ck_assert(pop->individuals[2]->genes[3][3] == genomes[2]->genes[3][3]);
	ck_assert(pop->individuals[2]->genes[3][4] == genomes[2]->genes[3][4]);

	population_destroy(pop);
	free(genomes);

}
END_TEST

START_TEST(test_mutation)
{
	int genes_count = 2;
	float genes[2][1] = {{2}, {3}};

	genome_t* genome = genome_create(genes_count, genes, *do_nothing, 1);

	for(int i = 0; i < 1000; ++i)
		mutate_genome(genome);

	ck_assert(genome->genes[0][0] != genes[0][0]);
	ck_assert(genome->genes[1][0] != genes[1][0]);

	genome_destroy(genome);
}
END_TEST

START_TEST(test_selection)
{
	int size = 3;
	float genes1[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
	float genes2[3][3] = {{11, 12, 13}, {14, 15, 16}, {17, 18, 19}};
	float genes3[3][3] = {{21, 22, 23}, {24, 25, 26}, {27, 28, 29}};	

	genome_t** genomes = SAFE_MALLOC(size*sizeof(genome_t*));
	genomes[0] = genome_create(5, genes1, *do_nothing, 5);
	genomes[1] = genome_create(5, genes2, *do_nothing, 5);
	genomes[2] = genome_create(5, genes3, *do_nothing, 5);

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
		
		if(ind == genes1)
			++count1;
		else if(ind == genes2)
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
	
	srunner_run_all(sr, CK_VERBOSE);

	n = srunner_ntests_failed (sr);
	srunner_free(sr);
	return(n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
