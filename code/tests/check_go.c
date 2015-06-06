#include <check.h>
#include <stdlib.h>
#include "go/board.h"
#include "go/record.h"
#include "go/game_controller.h"
#include "neuralnet/neuralnet.h"

board_t* board;
neuralnet_t* net1;
neuralnet_t* net2;

void setup (void)
{
    board = board_create (5);
    uint16_t buf_size = board->size * board->size;

    // TODO: use a deterministic initialition
    net1 = create_neural_net_random (buf_size, 5, 5, buf_size);
    net2 = create_neural_net_random (buf_size, 5, 5, buf_size);
}

void teardown (void)
{
    board_destroy (board);
}

// Tests for board.c
START_TEST (test_board_init)
{
    /* no null pointer */
    ck_assert (board != NULL);
    ck_assert (board->buffer != NULL);
    ck_assert (board->grid != NULL);

    /* correct size */
    ck_assert (board->size == 5);

    /* black begins */
    ck_assert (board->turn == c_black);

    /* no stones are captured */
    ck_assert (board->black_captured == 0);
    ck_assert (board->white_captured == 0);

    /* check grid pointer */
    ck_assert (board->grid[0][0] == ps_empty);
    ck_assert (board->grid[board->size - 1][0] == ps_empty);
    ck_assert (board->grid[0][board->size - 1] == ps_empty);
    ck_assert (board->grid[board->size - 1][board->size - 1] == ps_empty);
}
END_TEST

START_TEST (test_board_placement)
{
    ck_assert (board->turn == c_black);
    ck_assert (!board_legal_placement (board, 5, 0, c_black));
    ck_assert (!board_legal_placement (board, 6, 0, c_black));
    ck_assert (!board_legal_placement (board, 6, 6, c_black));
    ck_assert (board_legal_placement (board, 0, 0, c_black));
    ck_assert (!board_legal_placement (board, 0, 0, c_white));
    board_place (board, 0, 0);
    ck_assert (board->turn == c_white);
    ck_assert (!board_legal_placement (board, 0, 0, c_black));
    ck_assert (!board_legal_placement (board, 0, 0, c_white));
    ck_assert (!board_legal_placement (board, 1, 0, c_black));
    ck_assert (board_legal_placement (board, 1, 0, c_white));
    board_place (board, 1, 0);
    ck_assert (board->turn == c_black);
}
END_TEST

START_TEST (test_board_pass)
{
	uint8_t** before = board->grid;
    ck_assert (board->turn == c_black);
	board_pass(board);
	ck_assert(before == board->grid);
    ck_assert (board->turn == c_white);
}
END_TEST

START_TEST (test_board_liberties)
{
    board_place (board, 0, 0);
    ck_assert (board_num_liberties (board, board_get_group (board, 0, 0)) == 2);
    board_place (board, 1, 0);
    ck_assert (board_num_liberties (board, board_get_group (board, 0, 0)) == 1);
    ck_assert (board_num_liberties (board, board_get_group (board, 1, 0)) == 2);
    board_place (board, 0, 1);
    ck_assert (board_num_liberties (board, board_get_group (board, 0, 0)) == 2);
    ck_assert (board_num_liberties (board, board_get_group (board, 1, 0)) == 2);
    board_place (board, 1, 1);
    ck_assert (board_num_liberties (board, board_get_group (board, 0, 0)) == 1);
    ck_assert (board_num_liberties (board, board_get_group (board, 1, 0)) == 3);
}
END_TEST

START_TEST (test_board_groups)
{
    board_place (board, 1, 0);
    board_place (board, 0, 0);
    board_place (board, 2, 0);
	int** group = board_get_group(board, 1, 0);
	int** expected[][2] = {{0},{1, 0},{2, 0}};
    ck_assert (group == expected);
}
END_TEST

START_TEST (test_board_score)
{
    board_place (board, 0, 0);
    board_place (board, 1, 0);
    board_place (board, 2, 0);
    board_place (board, 0, 1);
    ck_assert(board_score(board, 9, 0) == -79);
}
END_TEST


// Tests for record.c
START_TEST (test_create_file)
{
    FILE* record = create_file ("0");
    ck_assert (record != NULL);
    fclose (record);
}
END_TEST

START_TEST (test_record_content)
{
    char* line = malloc (150);
    FILE* test_record = create_file ("1");

    write_move (test_record, 1, 2, 3);
    write_move (test_record, 0, 5, 3);
    write_move (test_record, 1, 4, 9);
    write_move (test_record, 0, 7, 1);

    ck_assert_str_eq (fgets (line, 150, test_record), "AP[nugengo:?]");
    ck_assert_str_eq (fgets (line, 150, test_record), "GM[1]");
    ck_assert_str_eq (fgets (line, 150, test_record), "SZ[9]");
    ck_assert_str_eq (fgets (line, 150, test_record), "B[bc]");
    ck_assert_str_eq (fgets (line, 150, test_record), "W[ec]");
    ck_assert_str_eq (fgets (line, 150, test_record), "B[di]");
    ck_assert_str_eq (fgets (line, 150, test_record), "W[ga]");

    fclose (test_record);
}
END_TEST


// Tests for game_controller.c
START_TEST (test_execute_move)
{
}
END_TEST

START_TEST (test_result_init)
{
    result_t test_result = result_init (net1, net2);
    ck_assert (test_result.black == net1);
    ck_assert (test_result.white == net2);
    ck_assert (test_result.score_black == 0);
    ck_assert (test_result.score_white == 0);
}
END_TEST


Suite* make_go_suite (void)
{
    Suite* s;
    TCase* tc_board;
    TCase* tc_record;
    TCase* tc_game;
    s = suite_create ("Go");

    /* Test case for board */
    tc_board = tcase_create ("Board");

    tcase_add_checked_fixture (tc_board, setup, teardown);
    tcase_add_test (tc_board, test_board_init);
    tcase_add_test (tc_board, test_board_placement);
	tcase_add_test (tc_board, test_board_pass);
    tcase_add_test (tc_board, test_board_liberties);
	tcase_add_test (tc_board, test_board_groups);
    tcase_add_test (tc_board, test_board_score);
    suite_add_tcase (s, tc_board);

    /* Test case for record */
    tc_record = tcase_create ("Record");

    tcase_add_test (tc_record, test_create_file);
    tcase_add_test (tc_record, test_record_content);
    suite_add_tcase (s, tc_record);

    /* Test case for game_controller */
    tc_game = tcase_create ("Game Controller");

    tcase_add_test (tc_game, test_execute_move);
    tcase_add_test (tc_game, test_result_init);
    suite_add_tcase (s, tc_game);

    return s;
}


int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_go_suite ());

    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
