#include "util/mpi.h"
#include "util/util.h"
#include <mpi.h>


void create_partition (partition_t* part, const process_info_t* pinfo, size_t n)
{
    size_t q = n / (size_t)pinfo->mpi_size;
    size_t r = n % (size_t)pinfo->mpi_size;
    part->start = (size_t)pinfo->mpi_rank * q;
    part->start += (size_t)pinfo->mpi_rank < r ? (size_t)pinfo->mpi_rank : r;
    part->end = (size_t)(pinfo->mpi_rank + 1) * q;
    part->end += (size_t)(pinfo->mpi_rank + 1) < r ? (size_t)(pinfo->mpi_rank + 1) : r;
}


int broadcast_neuralnet (neuralnet_t** netp, int root)
{
    MPI_Barrier (MPI_COMM_WORLD);
    int rc;
    int mpi_rank;
    MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);

    neuralnet_t* net;
    size_t layer_count;
    size_t* neurons_per_layer = NULL;
    float* edges = NULL;
    size_t edge_count;

    if (mpi_rank == root)
    {
        net = *netp;
        layer_count = net->layer_count;
        neurons_per_layer = net->neurons_per_layer;
        edges = net->edge_buf;
        edge_count = net->edge_count;
    }
    rc = MPI_Bcast (&layer_count, 1, MPI_UINT64_T, root, MPI_COMM_WORLD);
    rc = MPI_Bcast (&edge_count, 1, MPI_UINT64_T, root, MPI_COMM_WORLD);
    if (mpi_rank != root)
    {
        neurons_per_layer = SAFE_MALLOC (layer_count * sizeof (size_t));
        edges = SAFE_MALLOC (edge_count * sizeof (float));
    }
    rc = MPI_Bcast (neurons_per_layer, (int)layer_count, MPI_UINT64_T, root, MPI_COMM_WORLD);
    rc = MPI_Bcast (edges, (int)edge_count, MPI_FLOAT, root, MPI_COMM_WORLD);

    if (mpi_rank != root)
    {
        *netp = nnet_create_buffer (layer_count, neurons_per_layer, edges);
        free (neurons_per_layer);
        free (edges);
    }

    MPI_Barrier (MPI_COMM_WORLD);
    return rc;
}


int broadcast_nnet_set (nnet_set_t** setp, int root)
{
    int rc;
    int mpi_rank;
    MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);

    size_t size;
    nnet_set_t* set = NULL;

    if (mpi_rank == root)
    {
        set = *setp;
        size = set->size;
    }
    rc = MPI_Bcast (&size, 1, MPI_UINT64_T, root, MPI_COMM_WORLD);
    if (mpi_rank != root)
    {
        set = nnet_set_create (size);
    }

    for (size_t i = 0; i < size; ++i)
    {
        rc = broadcast_neuralnet (&(set->nets[i]), root);
    }

    if (mpi_rank != root)
        *setp = set;

    return rc;
}