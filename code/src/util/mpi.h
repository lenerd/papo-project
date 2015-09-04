#ifndef MPI_UTILS_H
#define MPI_UTILS_H

/**
 * \file
 * \brief Contains some datastructures an functionality to deal with
 * partitioning in an MPI environment.
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 */

#include <stddef.h>

/**
 * \brief Contains information about the MPI environment.
 */
typedef struct
{
    /** \brief ID-number of a process */
    int mpi_rank;
    /** \brief Total number of processes */
    int mpi_size;
} process_info_t;

/**
 * \brief Describes an interval in a double loop.
 */
typedef struct
{
    /** \brief Start of the outer loop. */
    size_t start_x;
    /** \brief Start of the inner loop. */
    size_t start_y;
    /** \brief Number of elements to process. */
    size_t len;
} partition_t;

/**
 * \brief Creates a partition of initial*n*n items.
 * \param part
 * \param pinfo
 * \param n
 * \param initial
 * \pre part != NULL
 * \pre pinfo != NULL
 * \pre 0.0 <= initial <= 1.0
 */
void create_initial_partition (partition_t* part, const process_info_t* pinfo,
                               size_t n, double initial);

#endif /* MPI_UTILS_H */
