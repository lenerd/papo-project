#ifndef NGG_TOOL_H
#define NGG_TOOL_H

#include <stdbool.h>
#include <stddef.h>

enum action
{
    none,
    create,
    gen_data,
    train,
};

typedef struct
{
    enum action action;

    char* in;
    char* out;
    bool b_in;
    bool b_out;

    size_t number_layer;
    size_t* number_neurons;

    size_t iterations;

    bool set_a;
    bool set_i;
    bool set_l;
    bool set_n;
    bool set_o;
    bool set_t;

    bool verbose;
} options_t;


void init_opts (options_t* opts);
void cleanup_opts (options_t* opts);

int create_networks (options_t* opts);
int train_networks (options_t* opts);


#endif /* NGG_TOOL_H */
