#define _GNU_SOURCE 1

#include "go/board.h"
#include "go/player.h"
#include "go/game.h"
#include "go/record.h"

#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>


int main (void)
{
    player_t* p1 = player_create_human ();
    // player_t* p2 = player_create_human ();
    //size_t neurons[] = {81, 81, 81, 81, 81};
    size_t neurons[] = {25, 25, 25, 25, 26};
    neuralnet_t* net = nnet_create_random(5, neurons);
    player_t* p2 = player_create_net (net);

    game_t* game = game_create (p1, p2, 5, 10);
    FILE* file = fopen ("record", "w");
    FILE* file2 = fopen ("game.sgf", "w");
    game_add_recorder (game, recorder_ascii_create(game->board, file));
    game_add_recorder (game, recorder_sgf_create(game->board, file2));

    while (!game->finished)
    {
        game_step(game);
        printf("\n");
        board_print (game->board, stdout);
        printf("\nScore: %" PRId64 "\n", game_score (game));
        printf("Moves: %" PRId64 "\n", game->move_cnt);
        usleep(200000);
    }

    printf("\nScore: %" PRId64 "\n", game_score (game));
    printf("Moves: %" PRId64 "\n", game->move_cnt);

    game_destroy (game);
    nnet_destroy (net);
    fclose (file);
    fclose (file2);

    return EXIT_SUCCESS;
}
