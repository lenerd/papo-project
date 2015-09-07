#define _GNU_SOURCE

#include "player.h"
#include "go/board.h"
#include "neuralnet/neuralnet.h"
#include "util/util.h"

#include <assert.h>
#include <readline/readline.h>
#include <stdlib.h>


static float* board_to_nnet_v0 (const board_t* board, color_t color)
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

static float* board_to_nnet_v1 (const board_t* board, color_t color)
{
    assert (board != NULL);

    float* in = SAFE_CALLOC (2 * board->buf_size, sizeof (float));

    pos_state_t friend = (pos_state_t) color;
    pos_state_t enemy = friend == ps_black ? ps_white : ps_black;

    for (size_t i = 0; i < 2 * board->buf_size; ++i)
    {
        if (board->buffer[i] == friend)
        {
            in[i] = 1.0f;
            ++i;
        }
        else if (board->buffer[i] == enemy)
        {
            ++i;
            in[i] = 1.0f;
        }
    }

    return in;
}

static float* board_to_nnet (const board_t* board, color_t color, netver_t ver)
{
    switch (ver)
    {
        case ver1:
            return board_to_nnet_v1 (board, color);
        case ver0:
        default:
            return board_to_nnet_v0 (board, color);

    }
}

#if 0
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
#endif

typedef struct
{
    const neuralnet_t* net;
    netver_t ver;
} netplayer_context_t;

static size_t get_maximum_and_replace(float* array, size_t size)
{
    size_t result = 0;
    float max = -1000.0f;
    for(size_t i = 0; i < size; ++i){
        if(array[i] > max){
            max = array[i];
            result = i;
        }
    }
    array[result] = -1000.0f;
    return result;
}

static position_t net_move (const player_t* player, const board_t* board)
{
    assert (player != NULL);
    assert (board != NULL);

    const netplayer_context_t* ctx = player->move_context;

    const neuralnet_t* net = ctx->net;
    netver_t ver = ctx->ver;

    if (ver == ver0)
        assert (net->neurons_per_layer[0] == board->buf_size);
    else if (ver == ver1)
        assert (net->neurons_per_layer[0] == 2 * board->buf_size);
    assert (net->neurons_per_layer[net->layer_count - 1] ==
            board->buf_size + 1);

    float* in = board_to_nnet (board, player->color, ver);
    float* out = nnet_calculate_output (net, in);

    size_t* idx = SAFE_MALLOC ((board->buf_size + 1) * sizeof (size_t));

    for (size_t i = 0; i < board->buf_size + 1; ++i)
        idx[i] = i;

    qsort_r (idx, board->buf_size + 1, sizeof (size_t), &cmp, out);

    position_t pos;
    for (size_t i = board->buf_size; i <= board->buf_size; --i)
    {
        if (idx[i] == board->buf_size)
        {
            pos.x = board->size;
            pos.y = board->size;
        }
        else
            pos = board_1d_to_2d (board, idx[i]);

        if (idx[i] == board->buf_size ||
            board_legal_placement (board, pos.x, pos.y, player->color))
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
        printf ("\nYou are black.\n");
    else
        printf ("\nYou are white.\n");

    printf ("\nEnter x-position (0-%zu,p): ", board->size - 1);
    line = readline(NULL);
    pass = strcmp (line, "p") ==  0;
    sscanf (line, "%zu", &pos.y);
    free (line);

    if (pass)
        return pos;

    printf ("Enter y-position (0-%zu,p): ", board->size - 1);
    line = readline (NULL);
    pass = strcmp (line, "p") ==  0;
    sscanf (line, "%zu", &pos.x);
    free (line);

    if (pass)
        return pos;

    while (!board_legal_placement (board, pos.x, pos.y, player->color))
    {
        printf ("The position %zu - %zu is invalid.\n", pos.x, pos.y);

        printf ("\nEnter x-position (0-%zu,p): ", board->size - 1);
        line = readline(NULL);
        pass = strcmp (line, "p") ==  0;
        sscanf (line, "%zu", &pos.y);
        free (line);

        if (pass)
            return pos;

        printf ("Enter y-position (0-%zu,p): ", board->size - 1);
        line = readline (NULL);
        pass = strcmp (line, "p") ==  0;
        sscanf (line, "%zu", &pos.x);
        free (line);

        if (pass)
            return pos;
    }
    return pos;
}


player_t* player_create (move_func move, void* context, player_destruct destructor)
{
    player_t* p = SAFE_MALLOC (sizeof (player_t));

    p->color = c_black;
    p->move = move;
    p->move_context = context;
    p->destructor = destructor;

    return p;
}

void player_destroy_net (player_t* player)
{
    free (player->move_context);
}

player_t* player_create_net (const neuralnet_t* net, netver_t ver)
{
    assert (net != NULL);

    netplayer_context_t* ctx = SAFE_MALLOC (sizeof(netplayer_context_t));
    ctx->net = net;
    ctx->ver = ver;

    return player_create (&net_move, ctx, player_destroy_net);
}


player_t* player_create_human (void)
{
    return player_create (&human_move, NULL, NULL);
}

void player_destroy (player_t* player)
{
    assert (player != NULL);

    if (player->destructor != NULL)
        player->destructor(player);

    free (player);
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
