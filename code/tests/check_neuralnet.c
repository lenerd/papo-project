#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "neuralnet/neuralnet.h"


START_TEST (test_misc)
{
    size_t size[] = {1, 3, 3, 7};
    ck_assert (nnet_edge_count (4, size) == 46);
    ck_assert (nnet_node_count (4, size) == 14);
}
END_TEST

START_TEST (test_construction)
{
    size_t size[] = {1, 3, 3, 7};
    neuralnet_t* net = nnet_create_random (4, size);

    ck_assert (net->layer_count == 4);
    ck_assert (net->edge_count == 46);
    ck_assert (memcmp (size, net->neurons_per_layer,
                       net->layer_count * sizeof (size_t)) == 0);

    nnet_destroy (net);
}
END_TEST

START_TEST (test_pointer)
{
    size_t size[] = {1, 3, 3, 7};
    neuralnet_t* net = nnet_create_random (4, size);

    ck_assert (net->edge_helper[0] - net->edge_buf == 0);
    ck_assert (net->edge_helper[1] - net->edge_buf == 3);
    ck_assert (net->edge_helper[2] - net->edge_buf == 6);
    ck_assert (net->edge_helper[3] - net->edge_buf == 9);
    ck_assert (net->edge_helper[4] - net->edge_buf == 12);
    ck_assert (net->edge_helper[5] - net->edge_buf == 15);
    ck_assert (net->edge_helper[6] - net->edge_buf == 18);
    ck_assert (net->edge_helper[7] - net->edge_buf == 25);
    ck_assert (net->edge_helper[8] - net->edge_buf == 32);
    ck_assert (net->edge_helper[9] - net->edge_buf == 39);

    ck_assert (net->edges[0] - net->edge_helper == 0);
    ck_assert (net->edges[1] - net->edge_helper == 2);
    ck_assert (net->edges[2] - net->edge_helper == 6);

    nnet_destroy (net);
}
END_TEST

START_TEST (test_nnet_set)
{
    nnet_set_t* set = nnet_set_create (5);
    ck_assert (set->size == 5);
    nnet_set_destroy (set);
}
END_TEST

START_TEST (test_output)
{
    ck_abort_msg ("not implemented");
#if 0
    uint32_t size[] = {2, 4};
    neuralnet_t* net = create_neural_net_random (4, size);

	float input[] = {1, 2};
	float* output = calculate_output(net, input);

	ck_assert(output[0] >= 0);
	
	destroy_neural_net(net);
#endif
}
END_TEST

START_TEST (test_full_output)
{
    ck_abort_msg ("not implemented");
}
END_TEST

START_TEST (test_backpropagation)
{
    ck_abort_msg ("not implemented");
    /*
    uint32_t input_count = 2;
    uint32_t hidden_layer_count = 1;
    uint32_t neurons_per_hidden_layer = 10;
    uint32_t output_count = 1;

    uint32_t trainings = 10;

    neuralnet_t* net = create_neural_net_random(input_count, hidden_layer_count,
    neurons_per_hidden_layer, output_count);

    for(uint32_t i = 0; i < trainings; ++i){
        */
    /*
    Random_value_mm mach sehr große, wilde Sachen, so im sechstelligen
    Bereich...
    float ins[] = { random_value_mm(0.0f, 5.0f), random_value_mm(0.0f, 5.0f) };
    float target[] = { ins[0] + ins[1] };
    printf("%0.2f und %0.2f \n", ins[0], ins[1]);
    */
    /*
    for(uint32_t j = 0; j < 10; ++j)
    {
        float ins[] = {j, j};
        float target[] = {j + j};
        //printf("Error: %0.2f + %0.2f = %0.2f \n", target[0],
calculate_output_new(net, ins)[0], target[0] - calculate_output_new(net,
ins)[0]);
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

START_TEST (test_file)
{
    nnet_set_t* sets[7] = {0, 0, 0, 0, 0, 0, 0};
    neuralnet_t* nets[7] = {0, 0, 0, 0, 0, 0, 0};
    sets[0] = nnet_set_create (1);

    size_t size[] = {1, 3, 3, 7};

    sets[0]->nets[0] = nnet_create_random (4, size);
    nnet_set_to_file (sets[0], "bin2", true);
    nnet_set_to_file (sets[0], "text2", false);
    sets[1] = nnet_set_from_file ("bin2", true);
    sets[2] = nnet_set_from_file ("text2", false);
    nnet_set_to_file (sets[1], "bin2text", false);
    nnet_set_to_file (sets[1], "bin2bin", true);
    nnet_set_to_file (sets[2], "text2text", false);
    nnet_set_to_file (sets[2], "text2bin", true);
    sets[3] = nnet_set_from_file ("bin2bin", true);
    sets[4] = nnet_set_from_file ("bin2text", false);
    sets[5] = nnet_set_from_file ("text2bin", true);
    sets[6] = nnet_set_from_file ("text2text", false);

    nets[0] = sets[0]->nets[0];
    nets[1] = sets[1]->nets[0];
    nets[2] = sets[2]->nets[0];
    nets[3] = sets[3]->nets[0];
    nets[4] = sets[4]->nets[0];
    nets[5] = sets[5]->nets[0];
    nets[6] = sets[6]->nets[0];

    ck_assert (nets[0]->layer_count == nets[1]->layer_count);
    ck_assert (memcmp (nets[0]->neurons_per_layer, nets[1]->neurons_per_layer,
                       nets[0]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[0]->edge_count == nets[1]->edge_count);
    ck_assert (memcmp (nets[0]->edge_buf, nets[1]->edge_buf,
                       nets[0]->edge_count * sizeof (float)) == 0);

    ck_assert (nets[0]->layer_count == nets[2]->layer_count);
    ck_assert (memcmp (nets[0]->neurons_per_layer, nets[2]->neurons_per_layer,
                       nets[0]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[0]->edge_count == nets[2]->edge_count);
    ck_assert (memcmp (nets[0]->edge_buf, nets[2]->edge_buf,
                       nets[0]->edge_count * sizeof (float)) == 0);

    ck_assert (nets[0]->layer_count == nets[3]->layer_count);
    ck_assert (memcmp (nets[0]->neurons_per_layer, nets[3]->neurons_per_layer,
                       nets[0]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[0]->edge_count == nets[3]->edge_count);
    ck_assert (memcmp (nets[0]->edge_buf, nets[3]->edge_buf,
                       nets[0]->edge_count * sizeof (float)) == 0);

    ck_assert (nets[0]->layer_count == nets[4]->layer_count);
    ck_assert (memcmp (nets[0]->neurons_per_layer, nets[4]->neurons_per_layer,
                       nets[0]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[0]->edge_count == nets[4]->edge_count);
    ck_assert (memcmp (nets[0]->edge_buf, nets[4]->edge_buf,
                       nets[0]->edge_count * sizeof (float)) == 0);

    ck_assert (nets[0]->layer_count == nets[5]->layer_count);
    ck_assert (memcmp (nets[0]->neurons_per_layer, nets[5]->neurons_per_layer,
                       nets[0]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[0]->edge_count == nets[5]->edge_count);
    ck_assert (memcmp (nets[0]->edge_buf, nets[5]->edge_buf,
                       nets[0]->edge_count * sizeof (float)) == 0);

    ck_assert (nets[0]->layer_count == nets[6]->layer_count);
    ck_assert (memcmp (nets[0]->neurons_per_layer, nets[6]->neurons_per_layer,
                       nets[0]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[0]->edge_count == nets[6]->edge_count);
    ck_assert (memcmp (nets[0]->edge_buf, nets[6]->edge_buf,
                       nets[0]->edge_count * sizeof (float)) == 0);

    for (int i = 0; i < 7; ++i)
        nnet_set_destroy (sets[i]);

    remove ("bin2");
    remove ("text2");
    remove ("bin2bin");
    remove ("bin2text");
    remove ("text2bin");
    remove ("text2text");
}
END_TEST

START_TEST (test_file2)
{
    nnet_set_t* sets[] = {0, 0, 0};
    neuralnet_t* nets[] = {0, 0, 0, 0, 0, 0};
    sets[0] = nnet_set_create (2);

    size_t size1[] = {4, 2};
    size_t size2[] = {1, 3, 3, 7};

    sets[0]->nets[0] = nnet_create_random (2, size1);
    sets[0]->nets[1] = nnet_create_random (4, size2);
    nnet_set_to_file (sets[0], "bin2", true);
    nnet_set_to_file (sets[0], "text2", false);
    sets[1] = nnet_set_from_file ("bin2", true);
    sets[2] = nnet_set_from_file ("text2", false);

    nets[0] = sets[0]->nets[0];
    nets[1] = sets[0]->nets[1];
    nets[2] = sets[1]->nets[0];
    nets[3] = sets[1]->nets[1];
    nets[4] = sets[2]->nets[0];
    nets[5] = sets[2]->nets[1];

    // bin
    ck_assert (nets[0]->layer_count == nets[2]->layer_count);
    ck_assert (memcmp (nets[0]->neurons_per_layer, nets[2]->neurons_per_layer,
                       nets[0]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[0]->edge_count == nets[2]->edge_count);
    ck_assert (memcmp (nets[0]->edge_buf, nets[2]->edge_buf,
                       nets[0]->edge_count * sizeof (float)) == 0);

    ck_assert (nets[0]->layer_count == nets[4]->layer_count);
    ck_assert (memcmp (nets[0]->neurons_per_layer, nets[4]->neurons_per_layer,
                       nets[0]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[0]->edge_count == nets[4]->edge_count);
    ck_assert (memcmp (nets[0]->edge_buf, nets[4]->edge_buf,
                       nets[0]->edge_count * sizeof (float)) == 0);

    // text
    ck_assert (nets[1]->layer_count == nets[3]->layer_count);
    ck_assert (memcmp (nets[1]->neurons_per_layer, nets[3]->neurons_per_layer,
                       nets[1]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[1]->edge_count == nets[3]->edge_count);
    ck_assert (memcmp (nets[1]->edge_buf, nets[3]->edge_buf,
                       nets[1]->edge_count * sizeof (float)) == 0);

    ck_assert (nets[1]->layer_count == nets[5]->layer_count);
    ck_assert (memcmp (nets[1]->neurons_per_layer, nets[5]->neurons_per_layer,
                       nets[1]->layer_count * sizeof (size_t)) == 0);
    ck_assert (nets[1]->edge_count == nets[5]->edge_count);
    ck_assert (memcmp (nets[1]->edge_buf, nets[5]->edge_buf,
                       nets[1]->edge_count * sizeof (float)) == 0);

    for (int i = 0; i < 3; ++i)
        nnet_set_destroy (sets[i]);

    remove ("bin2");
    remove ("text2");
}
END_TEST

Suite* make_neuralnet_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Neuralnet");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_misc);
    tcase_add_test (tc_core, test_construction);
    tcase_add_test (tc_core, test_pointer);
    tcase_add_test (tc_core, test_nnet_set);
    tcase_add_test (tc_core, test_output);
    tcase_add_test (tc_core, test_full_output);
    tcase_add_test (tc_core, test_backpropagation);
    tcase_add_test (tc_core, test_file);
    tcase_add_test (tc_core, test_file2);
    suite_add_tcase (s, tc_core);

    return s;
}


int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_neuralnet_suite ());

    srunner_set_fork_status (sr, CK_NOFORK);
    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
