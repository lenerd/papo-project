#define _GNU_SOURCE 1

#include "util/mpi.h"
#include "neuralnet/neuralnet.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>


int main (int argc, char** argv)
{
    process_info_t pinfo;
    MPI_Init (&argc, &argv);
    size_t neurons[] = {2, 2, 1};
    MPI_Comm_rank (MPI_COMM_WORLD, &pinfo.mpi_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &pinfo.mpi_size);

#if 0
    neuralnet_t * net = NULL;

    if (pinfo.mpi_rank == 0)
    {
        printf ("rank %d\n", pinfo.mpi_rank);
        net = nnet_create_random (3, neurons);
        nnet_to_file (net, stdout, false);
        putchar('\n');
        putchar('\n');
    }

    broadcast_neuralnet (&net, 0);

    if (pinfo.mpi_rank == 1)
    {
        printf ("rank %d\n", pinfo.mpi_rank);
        nnet_to_file (net, stdout, false);
    }

    nnet_destroy (net);
#else
    nnet_set_t * set = NULL;

    if (pinfo.mpi_rank == 0)
    {
        printf ("rank %d\n", pinfo.mpi_rank);
        set = nnet_set_create (2);
        set->nets[0] = nnet_create_random (3, neurons);
        set->nets[1] = nnet_create_random (3, neurons);
        nnet_to_file (set->nets[0], stdout, false);
        putchar('\n');
        nnet_to_file (set->nets[1], stdout, false);
        putchar('\n');
        putchar('\n');
    }

    broadcast_nnet_set (&set, 0);

    if (pinfo.mpi_rank == 1)
    {
        printf ("rank %d\n", pinfo.mpi_rank);
        nnet_to_file (set->nets[0], stdout, false);
        putchar('\n');
        nnet_to_file (set->nets[1], stdout, false);
    }

    nnet_set_destroy (set);
#endif

    MPI_Finalize();

    return EXIT_SUCCESS;
}
