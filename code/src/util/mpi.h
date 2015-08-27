#ifndef MPI_UTILS_H
#define MPI_UTILS_H

/**
 * \file
 * TODO:
 */

#include "neuralnet/neuralnet.h"
#include <stddef.h>

typedef struct
{
    int mpi_rank;
    int mpi_size;
}
process_info_t;
typedef struct
{
    size_t start;
    size_t end;
}
partition_t;

void create_partition (partition_t* part, const process_info_t* pinfo, size_t n);

int broadcast_neuralnet (neuralnet_t** net, int root);
int broadcast_nnet_set (nnet_set_t** setp, int root);


#endif /* MPI_UTILS_H */
