#include "game.h"
#include "util/util.h"

#include <stddef.h>
#include <stdint.h>


game_t* game_create (player_t* black, player_t* white, size_t board_size,
                     uint64_t move_limit)
{
    game_t* game = SAFE_MALLOC (sizeof (game_t));

    game->black = black;
    game->white = white;
    player_set_color (black, c_black);
    player_set_color (white, c_white);

    game->board = board_create (board_size);
    game->turn = c_black;

    game->finished = false;

    game->passed = false;

    game->move_cnt = 0;
    game->play_cnt = 0;
    game->pass_cnt = 0;
    game->move_limit = move_limit;

    return game;
}


void game_destroy (game_t* game)
{
    board_destroy (game->board);
    free (game);
}


void game_step (game_t* game)
{
    if (game->finished)
        return;

    player_t* p = game->turn == c_black ? game->black : game->white;

    position_t pos;
    pos = player_move (p, game->board);
    if (board_legal_placement (game->board, pos.x, pos.y, game->turn))
    {
        board_place (game->board, pos.x, pos.y);
        game->passed = false;
        ++game->play_cnt;
    }
    else
    {
        board_pass (game->board);
        ++game->pass_cnt;
        if (game->passed)
            game->finished = true;
        game->passed = true;
        // fprintf(stdout, "player %u passed\n", game->turn);
    }

    game->turn = game->turn == c_black ? c_white : c_black;
    ++game->move_cnt;
    if (game->move_cnt == game->move_limit)
        game->finished = true;
}


int64_t game_score (const game_t* game)
{
    return board_score (game->board);
}
