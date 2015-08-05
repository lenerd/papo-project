#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "neuralnet/neuralnet.h"
#include "training/training.h"

START_TEST(test_backpropagation)
{
	int* layers = malloc(3*sizeof(int));
	layers[0] = 4;
	layers[1] = 4;
	layers[2] = 2;

	neuralnet_t* net = create_neural_net_random(3, layers);

	dataset_t* t_back = generate_training_data("path", 2, c_black);

	backpropagate(net, t_back->data[0].input->buffer, t_back->data[0].expected);

	int* test_input = malloc(4*sizeof(int));
	test_input[0] = 2;
	test_input[1] = 1;
	test_input[2] = 0;
	test_input[3] = 0;

	float* test_output = calculate_output(net, test_input);

	ck_assert(test_output[0] < 0.25);
	ck_assert(test_output[1] < 0.25);	
}
END_TEST

START_TEST (test_expected_values)
{
    training_data_t data2;

    create_training_data(&data2, 2, c_black);
    board_place_color(data2.input, 0, 1, c_black);
    board_place_color(data2.input, 1, 0, c_white);

	// generate_expected_values(&data1);
	generate_expected_values(&data2);

	ck_assert(data2.expected[0] == 1);
	ck_assert(data2.expected[1] == 0);
	ck_assert(data2.expected[2] == 0);
	ck_assert(data2.expected[3] == 1);

    destroy_training_data(&data2);
}
END_TEST

START_TEST (test_generate_training_data)
{
	dataset_t* test2 = generate_training_data("../../src/training/data/2", 2, c_black);

	ck_assert(test2->size == 2);

    ck_assert(test2->data[0].input->grid[0][0] == ps_empty);
    ck_assert(test2->data[0].input->grid[0][1] == ps_black);
    ck_assert(test2->data[0].input->grid[1][0] == ps_white);
    ck_assert(test2->data[0].input->grid[1][1] == ps_empty);

    ck_assert(test2->data[0].expected[0] == 1);
    ck_assert(test2->data[0].expected[1] == 0);
    ck_assert(test2->data[0].expected[2] == 0);
    ck_assert(test2->data[0].expected[3] == 1);

    ck_assert(test2->data[1].input->grid[0][0] == ps_black);
    ck_assert(test2->data[1].input->grid[0][1] == ps_empty);
    ck_assert(test2->data[1].input->grid[1][0] == ps_empty);
    ck_assert(test2->data[1].input->grid[1][1] == ps_white);

    ck_assert(test2->data[1].expected[0] == 0);
    ck_assert(test2->data[1].expected[1] == 1);
    ck_assert(test2->data[1].expected[2] == 1);
    ck_assert(test2->data[1].expected[3] == 0);

    destroy_dataset(test2);
}
END_TEST

START_TEST (test_read_file)
{
    training_data_t data1, data2;
    FILE* fp;
    create_training_data(&data1, 9, c_black);
    fp = tmpfile();
    ck_assert(fp != NULL);
    input_from_file(&data1, fp);
   	for(int i = 0; i < 81; ++i)
   	{ 
   		ck_assert_msg(data1.input->buffer[i] == 0, "is %d", data1.input->buffer[i]);
   	}
   
   	fclose(fp);
    destroy_training_data(&data1);
   
    fp = tmpfile();
   	const char* move1 = "B[ab]";
   	const char* move2 = "W[ah]";
   	const char* move3 = "B[ed]";
   	
   	fputs(move1, fp);
   	fputs(move2, fp);
   	fputs(move3, fp);
   
   	rewind(fp);

    create_training_data(&data2, 9, c_black);

    input_from_file(&data2, fp);
   
   	for(int i = 0; i < 81; ++i)
   	{ 
   		if(i == 7)
   			ck_assert(data2.input->buffer[i] == 2);
   		else if(i == 1 || i == 39)
   			ck_assert(data2.input->buffer[i] == 1);
   		else
   			ck_assert(data2.input->buffer[i] == 0);
   	}
   
   	fclose(fp);
    destroy_training_data(&data2);
   	
}
END_TEST

Suite* make_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Training");

    tc_core = tcase_create ("Core");

    tcase_add_test(tc_core, test_backpropagation);
    tcase_add_test (tc_core, test_read_file);
	tcase_add_test (tc_core, test_generate_training_data);
	tcase_add_test (tc_core, test_expected_values);
    suite_add_tcase (s, tc_core);

    return s;
}


int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_suite ());

   //Uncomment if needed for debugging with gdb:
   //srunner_set_fork_status (sr, CK_NOFORK);

    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
