#include <check.h>
#include <stdlib.h>
#include "go/board.h"
#include "go/record.h"
#include "go/game_controller.h"
#include "newneunet/newneunet.h"

board_t* board;
neuralnet_t* net1;
neuralnet_t* net2;

void setup (void)
{
    board = board_create (5);

    int* layers = malloc(4 * sizeof(int));
    layers[0] = board->buf_size;
    layers[1] = 5;
    layers[2] = 5;
    layers[3] = 2;

    // TODO: use a deterministic initialition
    net1 = create_neural_net_random (4, layers);
    net2 = create_neural_net_random (4, layers);
}

void teardown (void)
{
    board_destroy (board);
    destroy_neural_net (net1);
    destroy_neural_net (net2);
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
    ck_assert (board->grid[1][0] == ps_empty);
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
    ck_assert (!(board->turn == c_white));
    board_place (board, 0, 0);
	ck_assert(board_position_state(board, 0, 0) == ps_black);
    ck_assert (board->turn == c_white);
    ck_assert (!board_legal_placement (board, 0, 0, c_black));
    ck_assert (!board_legal_placement (board, 0, 0, c_white));
    ck_assert (!(board->turn == c_black));
    ck_assert (board_legal_placement (board, 1, 0, c_white));
    board_place (board, 1, 0);
	ck_assert(board_position_state(board, 1, 0) == ps_white);
    ck_assert (board->turn == c_black);
}
END_TEST

START_TEST (test_board_suicide)
{
    board_place (board, 1, 0);
    board_pass (board);
    board_place (board, 0, 1);
    /* Board:
     *  0123
     * 0 B  
     * 1B   
     * 2    
     * 3    
     */
    ck_assert (board_test_suicide(board, 0, 0, c_white));
    ck_assert (!board_legal_placement(board, 0, 0, c_white));
    board_place (board, 0, 2);
    board_pass(board);
    board_place (board, 1, 1);
    board_pass(board);
    board_place (board, 2, 0);
    /* Board:
     *  0123
     * 0 BW 
     * 1BW  
     * 2W   
     * 3    
     */
    ck_assert (board_test_suicide(board, 0, 0, c_black));
    ck_assert (!board_legal_placement(board, 0, 0, c_black));
    board_pass(board);
    ck_assert (!board_test_suicide(board, 0, 0, c_white));
    ck_assert (board_legal_placement(board, 0, 0, c_white));
}
END_TEST

START_TEST (test_board_pass)
{
    uint8_t* buffer =
        malloc ((size_t)(board->size * board->size) * sizeof (uint8_t));
    memcpy (buffer, board->buffer,
            (size_t)(board->size * board->size) * sizeof (uint8_t));

    ck_assert (board->turn == c_black);

    board_pass (board);
    ck_assert (board->turn == c_white);
    ck_assert (
        memcmp (buffer, board->buffer,
                (size_t)(board->size * board->size) * sizeof (uint8_t)) == 0);

    board_pass (board);
    ck_assert (board->turn == c_black);
    ck_assert (
        memcmp (buffer, board->buffer,
                (size_t)(board->size * board->size) * sizeof (uint8_t)) == 0);

    free (buffer);
}
END_TEST

START_TEST (test_board_liberties)
{
    board_place (board, 0, 0);
    ck_assert_msg (board_num_liberties (board, board_get_group (board, 0, 0)) ==
                   2);
    board_place (board, 1, 0);
    ck_assert_msg (board_num_liberties (board, board_get_group (board, 0, 0)) ==
                   1);
    ck_assert_msg (board_num_liberties (board, board_get_group (board, 1, 0)) ==
                   2);
    board_place (board, 0, 1);
    ck_assert_msg (board_num_liberties (board, board_get_group (board, 0, 0)) ==
                   2);
    ck_assert_msg (board_num_liberties (board, board_get_group (board, 1, 0)) ==
                   2);
    board_place (board, 1, 1);
    ck_assert_msg (board_num_liberties (board, board_get_group (board, 0, 0)) ==
                   1);
    ck_assert_msg (board_num_liberties (board, board_get_group (board, 1, 0)) ==
                   3);
}
END_TEST

START_TEST (test_board_capture)
{
    board_place (board, 0, 0);
    board_place (board, 1, 0);
    board_place (board, 0, 1);
    board_place (board, 0, 2);
    board_place (board, 1, 1);
    board_place (board, 2, 1);
    board_pass (board);
    ck_assert (board_position_state (board, 0, 0) == ps_black);
    ck_assert (board_position_state (board, 0, 1) == ps_black);
    ck_assert (board_position_state (board, 1, 1) == ps_black);
    board_place (board, 1, 2);
    ck_assert (board_position_state (board, 0, 0) == ps_empty);
    ck_assert (board_position_state (board, 0, 1) == ps_empty);
    ck_assert (board_position_state (board, 1, 1) == ps_empty);
}
END_TEST

START_TEST (test_board_groups)
{
    ck_assert (board->turn == c_black);

    board_place (board, 1, 0);
    board_place (board, 0, 0);
    board_place (board, 1, 1);

    ck_assert (board_position_state (board, 1, 0) == ps_black);
    ck_assert (board_position_state (board, 0, 0) == ps_white);
    ck_assert (board_position_state (board, 1, 1) == ps_black);

    ck_assert (board_get_group (board, 1, 0) == board_get_group (board, 1, 1));
    ck_assert (board_get_group (board, 1, 0) <= board_2d_to_1d (board, 1, 0));
    ck_assert (board_get_group (board, 1, 1) <= board_2d_to_1d (board, 1, 1));
    ck_assert (board_get_group (board, 0, 0) == board_2d_to_1d (board, 0, 0));

    board_place (board, 0, 2);
    board_place (board, 2, 0);
    board_place (board, 0, 1);

    ck_assert (board_get_group (board, 0, 0) == board_get_group (board, 0, 1));
    ck_assert (board_get_group (board, 0, 0) == board_get_group (board, 0, 2));
    ck_assert (board_get_group (board, 2, 0) == board_get_group (board, 1, 0));
}
END_TEST


START_TEST (test_board_score)
{
    int score = 0;
    board_place (board, 0, 0);
    board_place (board, 1, 0);
    board_place (board, 2, 0);
    board_place (board, 0, 1);
    // 0WB00
    // W0000
    // 00000
    // 00000
    // 00000
    score = board_score(board);
    ck_assert_msg(score == -2, "calculated score ist %d", score);
    board_place (board, 1, 1);
    board_pass (board);
    board_place (board, 0, 2);
    board_pass (board);
    board_place (board, 0, 0);
    // B0B00
    // 0B000
    // B0000
    // 00000
    // 00000
    score = board_score(board);
    ck_assert_msg(score == 22, "calculated score ist %d", score);
    board_place (board, 4, 0);
    // B0B0W
    // 0B000
    // B0000
    // 00000
    // 00000
    score = board_score(board);
    ck_assert_msg(score == 3, "calculated score ist %d", score);
    board_pass (board);
    board_place (board, 0, 3);
    board_pass (board);
    board_place (board, 1, 2);
    board_pass (board);
    board_place (board, 2, 1);
    board_pass (board);
    board_place (board, 3, 0);
    // B0BWW
    // 0BW00
    // BW000
    // W0000
    // 00000
    score = board_score(board);
    ck_assert_msg(score == -11, "calculated score ist %d", score);
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
    
    fclose(test_record);
    test_record = fopen("1", "r");

    fgets (line, 150, test_record);
    ck_assert_str_eq (line, "AP[nugengo:?] \n");
    fgets (line, 150, test_record);
    ck_assert_str_eq (line, "GM[1] \n");
    fgets (line, 150, test_record);
    ck_assert_str_eq (line, "SZ[9] \n");
    fgets (line, 150, test_record);
    ck_assert_str_eq (line, "B[cd]\n");
    fgets (line, 150, test_record);
    ck_assert_str_eq (line, "W[fd]\n");
    fgets (line, 150, test_record);
    ck_assert_str_eq (line, "B[ej]\n");
    fgets (line, 150, test_record);
    ck_assert_str_eq (line, "W[hb]\n");

    free (line);
    fclose (test_record);
    remove ("1");
}
END_TEST


// Tests for game_controller.c
START_TEST(test_play)
{	
	    int* layers = malloc(4 * sizeof(int));
	    layers[0] = board->buf_size;
	    layers[1] = 5;
	    layers[2] = 5;
	    layers[3] = 2;

	neuralnet_t* black = create_neural_net_random(4, layers);
	neuralnet_t* white = create_neural_net_random(4, layers);
	
	FILE* test = create_file ("1");

	result_t* result = play(2, black, white, 4.5, test);

	ck_assert_msg(result->score_black == -result->score_white, "%d", result->score_black); 

	destroy_neural_net(black);
	destroy_neural_net(white);

	fclose(test);
	remove("1");
}
END_TEST

START_TEST (test_genmove)
{
    result_t* test_result = result_init (net1, net2);
	move_t* test_move = genmove(board, test_result);
	ck_assert(test_move->x > -2);
	ck_assert(test_move->x < 11);
	ck_assert(test_move->y > -2);
	ck_assert(test_move->y < 10);
}
END_TEST

START_TEST (test_result_init)
{
    result_t* test_result = result_init (net1, net2);
    ck_assert (test_result->black == net1);
    ck_assert (test_result->white == net2);
    ck_assert (test_result->score_black == 0);
    ck_assert (test_result->score_white == 0);
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
    tcase_add_test (tc_board, test_board_suicide);
	tcase_add_test (tc_board, test_board_pass);
    tcase_add_test (tc_board, test_board_liberties);
    tcase_add_test (tc_board, test_board_capture);
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
    tcase_add_checked_fixture (tc_game, setup, teardown);

    tcase_add_test (tc_game, test_play);
    tcase_add_test (tc_game, test_genmove);
    tcase_add_test (tc_game, test_result_init);
    suite_add_tcase (s, tc_game);

    return s;
}


int main (void)
{
    int n;
    SRunner* sr;

    sr = srunner_create (make_go_suite ());

	//Uncomment if needed for debugging with gdb:
	//srunner_set_fork_status (sr, CK_NOFORK);

	srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
