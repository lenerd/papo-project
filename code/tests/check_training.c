#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "neuralnet/neuralnet.h"
#include "training/training.h"

START_TEST (test_training_data)
{
    training_data_t* data = td_create (5, 5);
    ck_assert (data != NULL);
    ck_assert (data->input != NULL);
    ck_assert (data->expected != NULL);
    ck_assert (data->input_size = 5);
    ck_assert (data->output_size = 5);
    td_destroy (data);
}
END_TEST

START_TEST (test_dataset)
{
    dataset_t* set = dataset_create (10);
    ck_assert (set != NULL);
    ck_assert (set->data != NULL);
    set->data[0] = td_create (5, 5);
    dataset_destroy (set);
}
END_TEST

START_TEST (test_dataset_file)
{
    dataset_t* new;
    dataset_t* old = dataset_create (3);

    for (int i = 0; i < 3; ++i)
        old->data[i] = td_generate_nxn_nxnp1 (5);

    dataset_to_file (old, "dataset");
    new = dataset_from_file ("dataset");

    ck_assert (new->size == old->size);
    for (int i = 0; i < 3; ++i)
    {
        training_data_t* td_new = new->data[i];
        training_data_t* td_old = old->data[i];
        ck_assert (td_new != NULL);
        ck_assert (td_new->input_size == td_old->input_size);
        ck_assert (td_new->output_size == td_old->output_size);
        ck_assert (memcmp (td_new->input, td_old->input,
                           td_old->input_size * sizeof (float)) == 0);
        ck_assert (memcmp (td_new->expected, td_old->expected,
                           td_old->output_size * sizeof (float)) == 0);
    }

    dataset_destroy (new);
    dataset_destroy (old);

    remove ("dataset");
}
END_TEST

START_TEST (test_td_gen_nxn_nxnp1)
{
    training_data_t* data = td_generate_nxn_nxnp1 (5);
    ck_assert (data->input_size = 25);
    ck_assert (data->output_size = 25);
    for (size_t i = 0; i < 25; ++i)
    {
        float in = data->input[i];
        if (!(in < 0.0f || in > 0.0f))
            ck_assert (!(data->expected[i] < 1.0f || data->expected[i] > 1.0f));
        else if (!(in < 1.0f || in > 1.0f) || !(in < -1.0f || in > -1.0f))
            ck_assert (!(data->expected[i] < 0.0f || data->expected[i] > 0.0f));
        else
            ck_abort ();
    }
    ck_assert (data->expected[25] = 1.0f);
    td_destroy (data);
}
END_TEST

START_TEST (test_backpropagation)
{
#if 0
	int* layers = malloc(3*sizeof(int));
	layers[0] = 4;
	layers[1] = 4;
	layers[2] = 2;

	neuralnet_t* net = nnet_create_random (3, layers);

	dataset_t* t_back = generate_training_data("path", 2, c_black);

	nnet_backpropagate(net, t_back->data[0].input->buffer, t_back->data[0].expected);

	int* test_input = malloc(4*sizeof(int));
	test_input[0] = 2;
	test_input[1] = 1;
	test_input[2] = 0;
	test_input[3] = 0;

	float* test_output = nnet_calculate_output(net, test_input);

	ck_assert(test_output[0] < 0.25);
	ck_assert(test_output[1] < 0.25);
#endif
}
END_TEST

START_TEST (test_expected_values)
{
#if 0
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
#endif
}
END_TEST

START_TEST (test_generate_training_data)
{
#if 0
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
#endif
}
END_TEST

START_TEST (test_read_file)
{
#if 0
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

#endif
}
END_TEST

Suite* make_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Training");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_training_data);
    tcase_add_test (tc_core, test_dataset);
    tcase_add_test (tc_core, test_dataset_file);
    tcase_add_test (tc_core, test_td_gen_nxn_nxnp1);

    tcase_add_test (tc_core, test_backpropagation);
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

    // Uncomment if needed for debugging with gdb:
    srunner_set_fork_status (sr, CK_NOFORK);

    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
