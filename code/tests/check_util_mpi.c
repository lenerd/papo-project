#include <check.h>
#include <math.h>
#include <stdlib.h>
#include "util/mpi.h"
#include <stdio.h>


START_TEST (test_partition)
{
    process_info_t pinfo0 = { 0, 4 };
    process_info_t pinfo1 = { 1, 4 };
    process_info_t pinfo2 = { 2, 4 };
    process_info_t pinfo3 = { 3, 4 };
    size_t n = 15;

    partition_t part;

    create_partition (&part, &pinfo0, n);
    ck_assert (part.start == 0);
    ck_assert (part.end == 4);

    create_partition (&part, &pinfo1, n);
    ck_assert (part.start == 4);
    ck_assert (part.end == 8);

    create_partition (&part, &pinfo2, n);
    ck_assert (part.start == 8);
    ck_assert (part.end == 12);

    create_partition (&part, &pinfo3, n);
    ck_assert (part.start == 12);
    ck_assert (part.end == 15);

}
END_TEST


Suite* make_mpi_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("MPI");

    tc_core = tcase_create ("MPI");

    tcase_add_test (tc_core, test_partition);
    suite_add_tcase (s, tc_core);

    return s;
}

int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_mpi_suite ());

    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
