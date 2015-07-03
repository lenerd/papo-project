#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "neuralnet/neuralnet.h"
#include "training/training.h"

START_TEST (test_backpropagation)
{
	float* buffer = malloc(3*sizeof(float));
	buffer[0] = 0.5;	
	buffer[1] = 0.5;
	buffer[2] = 0.5;
	
	neuralnet_t* net = create_neural_net_buffer(1, 1, 1, 1, buffer);
	float threshold = 0.2;

	uint8_t* value1 = malloc(sizeof(uint8_t)); 
	uint8_t* value2 = malloc(sizeof(uint8_t));
	uint8_t* value3 = malloc(sizeof(uint8_t));
		
	value1[0] = 6;
	value2[0] = 4;
	value3[0] = 2;

	uint8_t* data[] = {value1, value2, value3};

	float* wanted = malloc(3* sizeof(float));

	wanted[0]=12;
	wanted[1]=8;
	wanted[2]=4;

	int data_size = 3;

	float* result = backpropagation(net, 1, threshold, data, data_size, wanted);	

	ck_assert(result[0] > 1.5);
	ck_assert(result[0] < 2.5);
}
END_TEST

START_TEST (test_generate_data)
{

	
}
END_TEST

START_TEST (test_read_file)
{
	FILE* fp = fopen("test.sgf", "a+");
	int* out = read_file(fp, 9);
	for(int i = 0; i < 81; ++i)
	{ 
		ck_assert_msg(out[i] == 0, "is %d", out[i]);
	}

	fclose(fp);

	remove("test.sgf");
	
	FILE* fq = fopen("test.sgf", "a+");
	const char* move1 = "B[ab]";
	const char* move2 = "W[ah]";
	const char* move3 = "B[ed]";
	
	fputs(move1, fq);
	fputs(move2, fq);
	fputs(move3, fq);

	rewind(fq);

	int* out2 = read_file(fq, 9);

	for(int i = 0; i < 81; ++i)
	{ 
		if(i == 7)
			ck_assert(out2[i] == 2);
		else if(i == 1 || i == 39)
			ck_assert(out2[i] == 1);
		else
			ck_assert(out2[i] == 0);
	}

	fclose(fq);
	
	remove("test.sgf");
	
}
END_TEST

Suite* make_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Training");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_read_file);
	tcase_add_test (tc_core, test_generate_data);
	tcase_add_test (tc_core, test_backpropagation);
    suite_add_tcase (s, tc_core);

    return s;
}


int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_suite ());

   //Uncomment if needed for debugging with gdb:
   srunner_set_fork_status (sr, CK_NOFORK);

    srunner_run_all (sr, CK_NORMAL);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
