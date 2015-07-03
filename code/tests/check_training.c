#include <check.h>
#include <stdlib.h>
#include "training/training.h"


START_TEST (test_read_file)
{
    ck_assert (say_true () == true);
    ck_assert (say_false () == false);
}
END_TEST

Suite* make_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Lib");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_read_file);
    suite_add_tcase (s, tc_core);

    return s;
}


int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_suite ());

    srunner_run_all (sr, CK_NORMAL);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
