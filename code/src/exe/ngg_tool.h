#ifndef NGG_TOOL_H
#define NGG_TOOL_H

#include "go/player.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum action
{
    none,
    calc,
    create,
    gen_data,
    train,
};

typedef struct
{
    enum action action;

    char* in_path;
    char* out_path;
    char* training_data_path;
    bool b_in;
    bool b_out;

    size_t number_layer;
    size_t* number_neurons;

    size_t board_size;
    netver_t ver;

    size_t n;

    bool set_a;
    bool set_i;
    bool set_l;
    bool set_n;
    bool set_o;
    bool set_s;
    bool set_t;

    bool verbose;
} options_t;


void init_opts (options_t* opts);
void cleanup_opts (options_t* opts);

int calculate (options_t* opts);
int create_networks (options_t* opts);
int generate_training_data (options_t* opts);
int train_networks (options_t* opts);


#endif /* NGG_TOOL_H */
