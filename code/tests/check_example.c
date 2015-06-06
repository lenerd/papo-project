#include <check.h>
#include <stdlib.h>
#include "example/example.h"


START_TEST (test_lib)
{
    ck_assert (say_true () == true);
    ck_assert (say_false () == false);
}
END_TEST

Suite* make_example_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Lib");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_lib);
    suite_add_tcase (s, tc_core);

    return s;
}


int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_example_suite ());

    srunner_run_all (sr, CK_NORMAL);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
