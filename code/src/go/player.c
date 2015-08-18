#define _GNU_SOURCE

#include "player.h"
#include "go/board.h"
#include "neuralnet/neuralnet.h"
#include "util/util.h"

#include <assert.h>
#include <readline/readline.h>
#include <stdlib.h>


static float* board_to_nnet (const board_t* board, color_t color)
{
    assert (board != NULL);

    float* in = SAFE_MALLOC (board->buf_size * sizeof (float));

    pos_state_t friend = (pos_state_t) color;
    pos_state_t enemy = friend == ps_black ? ps_white : ps_black;

    for (size_t i = 0; i < board->buf_size; ++i)
    {
        if (board->buffer[i] == ps_empty)
            in[i] = 0.0f;
        else if (board->buffer[i] == friend)
            in[i] = 1.0f;
        else if (board->buffer[i] == enemy)
            in[i] = -1.0f;
        else
            assert (false);
    }

    return in;
}

static int cmp (const void* a, const void* b, void* context)
{
    size_t pa = *(size_t*) a;
    size_t pb = *(size_t*) b;
    float* c = (float*) context;
    if (c[pa] > c[pb])
        return 1;
    else if (c[pa] < c[pb])
        return -1;
    else
        return 0;
}

static position_t net_move (const player_t* player, const board_t* board)
{
    assert (player != NULL);
    assert (board != NULL);

    neuralnet_t* net = (neuralnet_t*) player->move_context;

    assert (net->neurons_per_layer[0] == board->buf_size);
    assert (net->neurons_per_layer[net->layer_count - 1] == board->buf_size);

    float* in = board_to_nnet (board, player->color);
    float* out = nnet_calculate_output (net, in);
    size_t* idx = SAFE_MALLOC (board->buf_size * sizeof (size_t));

    for (size_t i = 0; i < board->buf_size; ++i)
        idx[i] = i;

    qsort_r (idx, board->buf_size, sizeof (size_t), &cmp, out);

    position_t pos;
    for (size_t i = board->buf_size - 1; i < board->buf_size; --i)
    {
        pos = board_1d_to_2d (board, idx[i]);
        if (board_legal_placement (board, pos.x, pos.y, player->color))
            break;
    }

    free (in);
    free (out);
    free (idx);

    return pos;
}

static position_t human_move (const player_t* player, const board_t* board)
{
    assert (player != NULL);
    assert (board != NULL);

    char* line;
    bool pass;
    position_t pos = {board->size, board->size};
    printf ("\n");
    board_print (board, stdout);

    if (player->color == c_black)
        printf ("You are black.\n");
    else
        printf ("You are white.\n");

    printf ("Enter row (0-%zu,p): ", board->size - 1);
    line = readline(NULL);
    pass = strcmp (line, "p") ==  0;
    sscanf (line, "%zu", &pos.x);
    free (line);

    if (pass)
        return pos;

    printf ("Enter column (0-%zu,p): ", board->size - 1);
    line = readline (NULL);
    pass = strcmp (line, "p") ==  0;
    sscanf (line, "%zu", &pos.y);
    free (line);

    if (pass)
        return pos;

    while (!board_legal_placement (board, pos.x, pos.y, player->color))
    {
        printf ("The position %zu - %zu is invalid.\n", pos.x, pos.y);

        line = readline("Enter row: ");
        pass = strcmp (line, "p") ==  0;
        sscanf (line, "%zu", &pos.x);
        free (line);
        if (pass)
            return pos;

        line = readline ("Enter column: ");
        pass = strcmp (line, "p") ==  0;
        sscanf (line, "%zu", &pos.y);
        free (line);
        if (pass)
            return pos;
    }
    return pos;
}


player_t* player_create (move_func move, const void* context)
{
    player_t* p = SAFE_MALLOC (sizeof (player_t));

    p->color = c_black;
    p->move = move;
    p->move_context = context;

    return p;
}

player_t* player_create_net (const neuralnet_t* net)
{
    assert (net != NULL);

    return player_create (&net_move, net);
}


player_t* player_create_human (void)
{
    return player_create (&human_move, NULL);
}

position_t player_move (const player_t* player, const board_t* board)
{
    assert (player != NULL);
    assert (board != NULL);

    return player->move (player, board);
}


void player_set_color (player_t* player, color_t color)
{
    assert (player != NULL);

    player->color = color;
}
