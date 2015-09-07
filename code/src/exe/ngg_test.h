#ifndef NGG_TEST_H
#define NGG_TEST_H

#include "go/player.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef struct
{
    char* in_path1;
    char* in_path2;
    bool b_in;

    size_t number_layer;
    size_t* number_neurons;

    size_t board_size;
    netver_t ver1;
    netver_t ver2;

    size_t n;

    bool set_i;
    bool set_j;
    bool set_s;
} options_t;


void init_opts (options_t* opts);

int test (options_t* opts);


#endif /* NGG_TEST_H */
