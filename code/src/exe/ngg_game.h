#ifndef NGG_GAME_H
#define NGG_GAME_H

#include "go/player.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    char* in_path;
    char* out_path;
    bool b_in;
    bool b_out;

    size_t board_size;

    netver_t ver;

    size_t n;

    unsigned seed;

    bool set_a;
    bool set_i;
    bool set_n;
    bool set_o;
    bool set_s;
    bool set_seed;

    bool verbose;
    bool human_readable;
} options_t;


void init_opts (options_t* opts);
void cleanup_opts (options_t* opts);

int unsupervised (options_t* opts);

#endif /* NGG_GAME_H */
