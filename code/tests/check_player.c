#include <check.h>
#include <stdlib.h>

#include "go/board.h"
#include "go/player.h"

board_t* board;
player_t* player;

void setup(void)
{

}

void teardown(void)
{

}

START_TEST (test_record_content)
{
    char* line = malloc (150);
    FILE* test_record = create_file ("1");

    write_move (test_record, 1, 2, 3);
    write_move (test_record, 0, 5, 3);
    write_move (test_record, 1, 4, 9);
    write_move (test_record, 0, 7, 1);

    fclose (test_record);
    test_record = fopen ("1", "r");

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

SRunner* make_player_suite(void)
{
	Suite* s;
	TCase* tc_player;

	s = suite_create("Player");

	tc_player = tcase_create("Player");

	tcase_add_checked_fixture (tc_board, setup, teardown);
    tcase_add_test (tc_board, test_board_init);

    suite_add_tcase(s, tc_player);

    return s;
}

int main(void)
{
	SRunner* sr = srunner_create(make_player_suite());

	// Uncomment if needed for debugging with gdb:
    srunner_set_fork_status (sr, CK_NOFORK);

    srunner_run_all (sr, CK_VERBOSE);

    n = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}