#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "newneunet/newneunet.h"


START_TEST (test_file)
{
	/*
    neuralnet_t* net1 = NULL;
    neuralnet_t* net2 = NULL;
    neuralnet_t* net3 = NULL;
    neuralnet_t* net4 = NULL;
    neuralnet_t* net5 = NULL;
    neuralnet_t* net6 = NULL;
    neuralnet_t* net7 = NULL;

    net1 = create_neural_net_random (1, 3, 3, 7);
    neural_net_to_file (net1, "bin2", true);
    neural_net_to_file (net1, "text2", false);
    net2 = neural_net_from_file ("bin2", true);
    net3 = neural_net_from_file ("text2", false);
    neural_net_to_file (net2, "bin2text", false);
    neural_net_to_file (net2, "bin2bin", true);
    neural_net_to_file (net3, "text2text", false);
    neural_net_to_file (net3, "text2bin", true);
    net4 = neural_net_from_file ("bin2bin", true);
    net5 = neural_net_from_file ("bin2text", false);
    net6 = neural_net_from_file ("text2bin", true);
    net7 = neural_net_from_file ("text2text", false);

    ck_assert (net1->edges_count == net2->edges_count);
    ck_assert (net1->input_count == net2->input_count);
    ck_assert (net1->hidden_layer_count == net2->hidden_layer_count);
    ck_assert (net1->neurons_per_hidden_layer ==
               net2->neurons_per_hidden_layer);
    ck_assert (net1->output_count == net2->output_count);
    ck_assert (memcmp (net1->edge_buf, net2->edge_buf,
                       net1->edges_count * sizeof (float)) == 0);

    ck_assert (net1->edges_count == net3->edges_count);
    ck_assert (net1->input_count == net3->input_count);
    ck_assert (net1->hidden_layer_count == net3->hidden_layer_count);
    ck_assert (net1->neurons_per_hidden_layer ==
               net3->neurons_per_hidden_layer);
    ck_assert (net1->output_count == net3->output_count);
    ck_assert (memcmp (net1->edge_buf, net3->edge_buf,
                       net1->edges_count * sizeof (float)) == 0);

    ck_assert (net1->edges_count == net4->edges_count);
    ck_assert (net1->input_count == net4->input_count);
    ck_assert (net1->hidden_layer_count == net4->hidden_layer_count);
    ck_assert (net1->neurons_per_hidden_layer ==
               net4->neurons_per_hidden_layer);
    ck_assert (net1->output_count == net4->output_count);
    ck_assert (memcmp (net1->edge_buf, net4->edge_buf,
                       net1->edges_count * sizeof (float)) == 0);

    ck_assert (net1->edges_count == net5->edges_count);
    ck_assert (net1->input_count == net5->input_count);
    ck_assert (net1->hidden_layer_count == net5->hidden_layer_count);
    ck_assert (net1->neurons_per_hidden_layer ==
               net5->neurons_per_hidden_layer);
    ck_assert (net1->output_count == net5->output_count);
    ck_assert (memcmp (net1->edge_buf, net5->edge_buf,
                       net1->edges_count * sizeof (float)) == 0);

    ck_assert (net1->edges_count == net6->edges_count);
    ck_assert (net1->input_count == net6->input_count);
    ck_assert (net1->hidden_layer_count == net6->hidden_layer_count);
    ck_assert (net1->neurons_per_hidden_layer ==
               net6->neurons_per_hidden_layer);
    ck_assert (net1->output_count == net6->output_count);
    ck_assert (memcmp (net1->edge_buf, net6->edge_buf,
                       net1->edges_count * sizeof (float)) == 0);

    ck_assert (net1->edges_count == net7->edges_count);
    ck_assert (net1->input_count == net7->input_count);
    ck_assert (net1->hidden_layer_count == net7->hidden_layer_count);
    ck_assert (net1->neurons_per_hidden_layer ==
               net7->neurons_per_hidden_layer);
    ck_assert (net1->output_count == net7->output_count);
    ck_assert (memcmp (net1->edge_buf, net7->edge_buf,
                       net1->edges_count * sizeof (float)) == 0);

    destroy_neural_net (net1);
    destroy_neural_net (net2);
    destroy_neural_net (net3);
    destroy_neural_net (net4);
    destroy_neural_net (net5);
    destroy_neural_net (net6);
    destroy_neural_net (net7);
    remove ("bin2");
    remove ("text2");
    remove ("bin2bin");
    remove ("bin2text");
    remove ("text2bin");
    remove ("text2text");
	*/
}
END_TEST

Suite* make_neuralnet_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Neuralnet");

    tc_core = tcase_create ("Core");

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
    srunner_run_all (sr, CK_NORMAL);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
