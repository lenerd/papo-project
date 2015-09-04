#include "util/mpi.h"
#include "util/util.h"
#include <assert.h>


void create_initial_partition (partition_t* part, const process_info_t* pinfo, size_t n, double initial)
{
    assert (part != NULL);
    assert (pinfo != NULL);
    assert (0.0 <= initial);
    assert (1.0 >= initial);

    size_t mpi_size = (size_t) pinfo->mpi_size - 1;
    size_t mpi_rank = (size_t) pinfo->mpi_rank - 1;

    size_t problem_size = (size_t)((double)(n * n) * initial);
    size_t q = problem_size / mpi_size;
    size_t r = problem_size % mpi_size;
    size_t start = mpi_rank * q;
    start += mpi_rank < r ? mpi_rank : r;
    size_t len = q;
    len += mpi_rank < r ? 1 : 0;

    part->start_x = start / n;
    part->start_y = start % n;
    part->len = len;
}
