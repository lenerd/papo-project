#define _GNU_SOURCE 1

#include "go/board.h"
#include "go/player.h"
#include "go/game.h"

#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>


int main (void)
{
    player_t* p1 = player_create_human ();
    // player_t* p2 = player_create_human ();
    //size_t neurons[] = {81, 81, 81, 81, 81};
    size_t neurons[] = {25, 25, 25, 25, 25};
    neuralnet_t* net = nnet_create_random(5, neurons);
    player_t* p2 = player_create_net (net);

    game_t* game = game_create (p1, p2, 5, 1024);

    while (!game->finished)
    {
        game_step(game);
        board_print (game->board, stdout);
        printf("Score: %" PRId64 "\n", game_score (game));
        printf("Moves: %" PRId64 "\n", game->move_cnt);
        usleep(200000);
    }

    printf("Score: %" PRId64 "\n", game_score (game));
    printf("Moves: %" PRId64 "\n", game->move_cnt);

    game_destroy (game);
    nnet_destroy (net);

    return EXIT_SUCCESS;
}
