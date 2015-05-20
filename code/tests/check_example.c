#include <check.h>
#include <stdlib.h>
#include "example/example.h"


START_TEST (test_lib)
{
    ck_assert(say_true() == true);
    ck_assert(say_false() == false);
}
END_TEST

Suite* lib_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create("Lib");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_lib);
    suite_add_tcase(s, tc_core);

    return s;
}


int main (void)
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = lib_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
