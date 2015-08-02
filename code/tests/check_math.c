#define _DEFAULT_SOURCE
#include <check.h>
#include <math.h>
#include <stdlib.h>
#include "util/math_ext.h"

const unsigned int seed = 0xDEADBEEF;


START_TEST (test_random)
{
    float val;
    ck_assert (seed_set == false);
    set_seed (seed);
    ck_assert (seed_set == true);
    for (int i = 0; i < 10; ++i)
    {
        val = random_value_01 ();
        ck_assert (0.0f <= val && val <= 1.0f);
    }
    for (int i = 0; i < 10; ++i)
    {
        val = random_value_0m (42.1337f);
        ck_assert (0.0f <= val && val <= 42.1337f);
    }
    for (int i = 0; i < 10; ++i)
    {
        val = random_value_mm (42.1337f, 1337.42f);
        ck_assert (42.1337f <= val && val <= 1337.42f);
    }
}
END_TEST

START_TEST (test_sigmoid)
{
    float val;
    float comp;
    val = sigmoid (0.0f);
    comp = 0.5f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
    val = sigmoid (1.0f);
    comp = 0.7310585786300049f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
    val = sigmoid (0.1337f);
    comp = 0.5333752975997255f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
    val = sigmoid (0.42f);
    comp = 0.6034832498647263f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
    val = sigmoid (INFINITY);
    comp = 1.0f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
    val = sigmoid (-INFINITY);
    comp = 0.0f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
}
END_TEST

START_TEST (test_inverse_sigmoid)
{
    float val;
    float comp;
    val = inverse_sigmoid (0.0f);
    comp = -INFINITY;
    ck_assert_msg (isinff (val) == -1, "%f != %f", (double) val, (double) comp);
    val = inverse_sigmoid (1.0f);
    comp = INFINITY;
    ck_assert_msg (isinff (val) == 1, "%f != %f", (double) val, (double) comp);
    val = inverse_sigmoid (0.1f);
    comp = -2.1972245773362196f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
    val = inverse_sigmoid (0.9f);
    comp = 2.197224577336219f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
    val = inverse_sigmoid (0.1337f);
    comp = -1.868632784788197f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
    val = inverse_sigmoid (0.42f);
    comp = -0.322773392263051f;
    ck_assert_msg (fabsf (val - comp) < 0.000001f, "%f != %f", (double) val,
                   (double) comp);
}
END_TEST

Suite* make_random_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Math");

    tc_core = tcase_create ("Random");

    tcase_add_test (tc_core, test_random);
    suite_add_tcase (s, tc_core);

    return s;
}

Suite* make_sigmoid_suite (void)
{
    Suite* s;
    TCase* tc_core;
    s = suite_create ("Math");

    tc_core = tcase_create ("Sigmoid");

    tcase_add_test (tc_core, test_sigmoid);
    tcase_add_test (tc_core, test_inverse_sigmoid);
    suite_add_tcase (s, tc_core);

    return s;
}

int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_random_suite ());
    srunner_add_suite (sr, make_sigmoid_suite ());

    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
