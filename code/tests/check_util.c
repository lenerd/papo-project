#include <check.h>
#include <math.h>
#include <stdlib.h>
#include "util/util.h"
#include <stdio.h>


START_TEST (test_time_diff)
{
    struct timespec a, b, c;

    a.tv_sec = 13;
    a.tv_nsec = 370000000;

    b.tv_sec = 42;
    b.tv_nsec = 170100000;

    c = diff_timespec (a, b);

    ck_assert (c.tv_sec == 28);
    ck_assert (c.tv_nsec == 800100000);
}
END_TEST

START_TEST (test_time_sum)
{
    struct timespec a, b, c;

    a.tv_sec = 13;
    a.tv_nsec = 370000000;

    b.tv_sec = 42;
    b.tv_nsec = 170100000;

    c = sum_timespec (a, b);

    ck_assert (c.tv_sec == 55);
    ck_assert (c.tv_nsec == 540100000);
}
END_TEST

START_TEST (test_time_div)
{
    struct timespec a, b;

    a.tv_sec = 13;
    a.tv_nsec = 370000000;

    b = div_timespec (a, 1000);

    ck_assert (b.tv_sec == 0);
    fprintf (stderr, "%lu\n", b.tv_nsec);
    ck_assert (b.tv_nsec == 13370000);
}
END_TEST


Suite* make_time_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Util");

    tc_core = tcase_create ("Time");

    tcase_add_test (tc_core, test_time_diff);
    tcase_add_test (tc_core, test_time_sum);
    tcase_add_test (tc_core, test_time_div);
    suite_add_tcase (s, tc_core);

    return s;
}

int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_time_suite ());

    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
