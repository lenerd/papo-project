#include "util/misc.h"
#include "go/player.h"

int check_board_size (nnet_set_t* set, size_t board_size, netver_t ver)
{
    if (ver == ver0)
    {
        size_t in_cnt = board_size * board_size;
        size_t out_cnt = board_size * board_size + 1;
        for (size_t i = 0; i < set->size; ++i)
        {
            if (set->nets[i]->neurons_per_layer[0] != in_cnt ||
                set->nets[i]->neurons_per_layer[set->nets[i]->layer_count -
                                                1] != out_cnt)
            {
                fprintf (stderr,
                         "net #%zu has an illegal number of i/o neurons\n", i);
                return 1;
            }
        }
    }
    else if (ver == ver1)
    {
        size_t in_cnt = 2 * board_size * board_size;
        size_t out_cnt = board_size * board_size + 1;
        for (size_t i = 0; i < set->size; ++i)
        {
            if (set->nets[i]->neurons_per_layer[0] != in_cnt ||
                set->nets[i]->neurons_per_layer[set->nets[i]->layer_count -
                                                1] != out_cnt)
            {
                fprintf (stderr,
                         "net #%zu has an illegal number of i/o neurons\n", i);
                return 1;
            }
        }
    }
    return 0;
}
