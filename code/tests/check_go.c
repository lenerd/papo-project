#include <check.h>
#include <stdlib.h>
#include "go/board.h"

board_t* board;

void setup (void)
{
    board = board_create(5);
}

void teardown (void)
{
    board_destroy(board);
}

START_TEST (test_board_init)
{
    /* no null pointer */
    ck_assert(board != NULL);
    /* black begins */
    ck_assert(board->turn == c_black);
}
END_TEST

START_TEST (test_board_placement)
{
    ck_assert(board->turn == c_black);
    ck_assert(board_legal_placement(board, 0, 0, c_black));
    ck_assert(!board_legal_placement(board, 0, 0, c_white));
    board_place(board, 0, 0, c_black);
    ck_assert(board->turn == c_white);
    ck_assert(!board_legal_placement(board, 0, 0, c_black));
    ck_assert(!board_legal_placement(board, 0, 0, c_white));
    ck_assert(!board_legal_placement(board, 1, 0, c_black));
    ck_assert(board_legal_placement(board, 1, 0, c_white));
    board_place(board, 1, 0, c_white);
    ck_assert(board->turn == c_black);
}
END_TEST

START_TEST (test_board_capture)
{
    board_place(board, 0, 0, c_black);
    board_place(board, 1, 0, c_white);
    board_place(board, 2, 0, c_black);
    board_place(board, 0, 1, c_white);
    ck_assert(board_position_state(board, 0, 0) == ps_empty);
}
END_TEST

Suite* board_suite (void)
{
    Suite* s;
    TCase* tc_board;
    s = suite_create("Go");

    tc_board = tcase_create("Board");

    tcase_add_test(tc_board, test_board_init);
    tcase_add_test(tc_board, test_board_placement);
    suite_add_tcase(s, tc_board);

    return s;
}


int main (void)
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = board_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
