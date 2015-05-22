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
    ck_assert(board->buffer != NULL);
    ck_assert(board->grid != NULL);

    /* correct size */
    ck_assert(board->size == 5);

    /* black begins */
    ck_assert(board->turn == c_black);

    /* no stones are captured */
    ck_assert(board->black_captured == 0);
    ck_assert(board->white_captured == 0);

    /* check grid pointer */
    ck_assert(board->grid[0][0] == ps_empty);
    ck_assert(board->grid[-1][0] == ps_illegal);
    ck_assert(board->grid[0][-1] == ps_illegal);
    ck_assert(board->grid[-1][-1] == ps_illegal);
    ck_assert(board->grid[board->size - 1][0] == ps_empty);
    ck_assert(board->grid[0][board->size - 1] == ps_empty);
    ck_assert(board->grid[board->size - 1][board->size - 1] == ps_empty);
    ck_assert(board->grid[board->size][0] == ps_illegal);
    ck_assert(board->grid[0][board->size] == ps_illegal);
    ck_assert(board->grid[board->size][board->size] == ps_illegal);
}
END_TEST

START_TEST (test_board_placement)
{
    ck_assert(board->turn == c_black);
    ck_assert(!board_legal_placement(board, 5, 0, c_black));
    ck_assert(!board_legal_placement(board, 6, 0, c_black));
    ck_assert(!board_legal_placement(board, 6, 6, c_black));
    ck_assert(board_legal_placement(board, 0, 0, c_black));
    ck_assert(!board_legal_placement(board, 0, 0, c_white));
    board_place(board, 0, 0);
    ck_assert(board->turn == c_white);
    ck_assert(!board_legal_placement(board, 0, 0, c_black));
    ck_assert(!board_legal_placement(board, 0, 0, c_white));
    ck_assert(!board_legal_placement(board, 1, 0, c_black));
    ck_assert(board_legal_placement(board, 1, 0, c_white));
    board_place(board, 1, 0);
    ck_assert(board->turn == c_black);
}
END_TEST

START_TEST (test_board_capture)
{
    board_place(board, 0, 0);
    board_place(board, 1, 0);
    board_place(board, 2, 0);
    board_place(board, 0, 1);
    ck_assert(board_position_state(board, 0, 0) == ps_empty);
}
END_TEST

Suite* board_suite (void)
{
    Suite* s;
    TCase* tc_board;
    s = suite_create("Go");

    tc_board = tcase_create("Board");

    tcase_add_checked_fixture(tc_board, setup, teardown);
    tcase_add_test(tc_board, test_board_init);
    tcase_add_test(tc_board, test_board_placement);
    tcase_add_test(tc_board, test_board_capture);
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
