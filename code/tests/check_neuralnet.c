#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "neuralnet/neuralnet.h"


START_TEST (test_misc)
{
    size_t size[] = {1, 3, 3, 7};
    ck_assert (nnet_edge_count (4, size) == 33);
    ck_assert (nnet_node_count (4, size) == 14);
}
END_TEST

START_TEST (test_construction)
{
    size_t size[] = {1, 3, 3, 7};
    neuralnet_t* net = nnet_create_random (4, size);

    ck_assert (net->layer_count == 4);
    ck_assert (net->edge_count == 33);
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
    ck_assert (net->edge_helper[5] - net->edge_buf == 19);
    ck_assert (net->edge_helper[6] - net->edge_buf == 26);

    ck_assert (net->edges[0] - net->edge_helper == 0);
    ck_assert (net->edges[1] - net->edge_helper == 1);
    ck_assert (net->edges[2] - net->edge_helper == 4);

    nnet_destroy (net);
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
    neuralnet_t* net1 = NULL;
    neuralnet_t* net2 = NULL;
    neuralnet_t* net3 = NULL;
    neuralnet_t* net4 = NULL;
    neuralnet_t* net5 = NULL;
    neuralnet_t* net6 = NULL;
    neuralnet_t* net7 = NULL;
    size_t size[] = {1, 3, 3, 7};

    net1 = nnet_create_random (4, size);
    nnet_to_file (net1, "bin2", true);
    nnet_to_file (net1, "text2", false);
    net2 = nnet_from_file ("bin2", true);
    net3 = nnet_from_file ("text2", false);
    nnet_to_file (net2, "bin2text", false);
    nnet_to_file (net2, "bin2bin", true);
    nnet_to_file (net3, "text2text", false);
    nnet_to_file (net3, "text2bin", true);
    net4 = nnet_from_file ("bin2bin", true);
    net5 = nnet_from_file ("bin2text", false);
    net6 = nnet_from_file ("text2bin", true);
    net7 = nnet_from_file ("text2text", false);

    ck_assert (net1->layer_count == net2->layer_count);
    ck_assert (memcmp (net1->neurons_per_layer, net2->neurons_per_layer,
                       net1->layer_count * sizeof (uint32_t)) == 0);
    ck_assert (net1->edge_count == net2->edge_count);
    ck_assert (memcmp (net1->edge_buf, net2->edge_buf,
                       net1->edge_count * sizeof (float)) == 0);

    ck_assert (net1->layer_count == net3->layer_count);
    ck_assert (memcmp (net1->neurons_per_layer, net3->neurons_per_layer,
                       net1->layer_count * sizeof (uint32_t)) == 0);
    ck_assert (net1->edge_count == net3->edge_count);
    ck_assert (memcmp (net1->edge_buf, net3->edge_buf,
                       net1->edge_count * sizeof (float)) == 0);

    ck_assert (net1->layer_count == net4->layer_count);
    ck_assert (memcmp (net1->neurons_per_layer, net4->neurons_per_layer,
                       net1->layer_count * sizeof (uint32_t)) == 0);
    ck_assert (net1->edge_count == net4->edge_count);
    ck_assert (memcmp (net1->edge_buf, net4->edge_buf,
                       net1->edge_count * sizeof (float)) == 0);

    ck_assert (net1->layer_count == net5->layer_count);
    ck_assert (memcmp (net1->neurons_per_layer, net5->neurons_per_layer,
                       net1->layer_count * sizeof (uint32_t)) == 0);
    ck_assert (net1->edge_count == net5->edge_count);
    ck_assert (memcmp (net1->edge_buf, net5->edge_buf,
                       net1->edge_count * sizeof (float)) == 0);

    ck_assert (net1->layer_count == net6->layer_count);
    ck_assert (memcmp (net1->neurons_per_layer, net6->neurons_per_layer,
                       net1->layer_count * sizeof (uint32_t)) == 0);
    ck_assert (net1->edge_count == net6->edge_count);
    ck_assert (memcmp (net1->edge_buf, net6->edge_buf,
                       net1->edge_count * sizeof (float)) == 0);

    ck_assert (net1->layer_count == net7->layer_count);
    ck_assert (memcmp (net1->neurons_per_layer, net7->neurons_per_layer,
                       net1->layer_count * sizeof (uint32_t)) == 0);
    ck_assert (net1->edge_count == net7->edge_count);
    ck_assert (memcmp (net1->edge_buf, net7->edge_buf,
                       net1->edge_count * sizeof (float)) == 0);

    nnet_destroy (net1);
    nnet_destroy (net2);
    nnet_destroy (net3);
    nnet_destroy (net4);
    nnet_destroy (net5);
    nnet_destroy (net6);
    nnet_destroy (net7);
    remove ("bin2");
    remove ("text2");
    remove ("bin2bin");
    remove ("bin2text");
    remove ("text2bin");
    remove ("text2text");
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
    tcase_add_test (tc_core, test_output);
    tcase_add_test (tc_core, test_full_output);
    tcase_add_test (tc_core, test_backpropagation);
    tcase_add_test (tc_core, test_file);
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
