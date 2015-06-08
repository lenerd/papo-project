#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "neuralnet/neuralnet.h"


START_TEST (test_file)
{
    neuralnet_t* net1 = NULL;
    neuralnet_t* net2 = NULL;
    neuralnet_t* net3 = NULL;
    const char* filename_bin = "./testfile.bin.nn";
    const char* filename_text = "./testfile.text.nn";

    net1 = create_neural_net_random (1, 3, 3, 7);
    neural_net_to_file (net1, filename_bin, true);
    neural_net_to_file (net1, filename_text, false);
    net2 = neural_net_from_file (filename_bin, true);
    net3 = neural_net_from_file (filename_text, false);

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

    destroy_neural_net (net1);
    destroy_neural_net (net2);
    destroy_neural_net (net3);
    remove (filename_bin);
    remove (filename_text);
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

    srunner_run_all (sr, CK_NORMAL);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
