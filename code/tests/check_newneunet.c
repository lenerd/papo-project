#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "math.h"
#include "newneunet/newneunet.h"		

START_TEST(test_construction)
{
	//neuralnet_t* net = create_neural_net_random(2, 2, 3, 1);

	/*
	ck_assert(net->input_count == 2);
	ck_assert(net->output_count == 1);
	ck_assert(net->hidden_layer_count == 2);
	ck_assert(net->neurons_per_hidden_layer == 3);
	*/

	//destroy_neural_net(net);

	//ck_assert(net == NULL);
}
END_TEST

START_TEST(test_output)
{
	/*
	neuralnet_t* net = create_neural_net_random(2, 2, 3, 1);

	float input[] = {1, 2};
	float* output = calculate_output(net, input);

	ck_assert(output[0] >= 0);
	
	destroy_neural_net(net);
	*/
}
END_TEST

START_TEST(test_full_output)
{
}
END_TEST

START_TEST (test_backpropagation)
{	
	/*
	uint32_t input_count = 2;
	uint32_t hidden_layer_count = 1;
	uint32_t neurons_per_hidden_layer = 10;
	uint32_t output_count = 1;

	uint32_t trainings = 10;

	neuralnet_t* net = create_neural_net_random(input_count, hidden_layer_count, neurons_per_hidden_layer, output_count);

	for(uint32_t i = 0; i < trainings; ++i){
		*/
		/*
		Random_value_mm mach sehr große, wilde Sachen, so im sechstelligen Bereich...
		float ins[] = { random_value_mm(0.0f, 5.0f), random_value_mm(0.0f, 5.0f) };
		float target[] = { ins[0] + ins[1] };
		printf("%0.2f und %0.2f \n", ins[0], ins[1]);
		*/
		/*
		for(uint32_t j = 0; j < 10; ++j)
		{
			float ins[] = {j, j};
			float target[] = {j + j};
			//printf("Error: %0.2f + %0.2f = %0.2f \n", target[0], calculate_output_new(net, ins)[0], target[0] - calculate_output_new(net, ins)[0]);
			backpropagate(net, ins, target);
		}

	}

	const float input1[] = {2, 2};
	const float input2[] = {2, 3};
	const float input3[] = {5, 12};

	float* test1 = calculate_output(net, input1);
	float* test2 = calculate_output(net, input2);
	float* test3 = calculate_output(net, input3);


	ck_assert(test1[0] > 3.5);
	ck_assert(test1[0] < 4.5);
	ck_assert(test2[0] > 4.5);
	ck_assert(test2[0] < 5.5);
	ck_assert(test3[0] > 16.5);
	ck_assert(test3[0] < 17.5);

	destroy_neural_net(net);
	*/
	
}
END_TEST

Suite* make_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Newneunet");

    tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_construction);
	tcase_add_test (tc_core, test_output);
	tcase_add_test (tc_core, test_full_output);
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

    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


