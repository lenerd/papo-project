#ifndef UTIL_MISC_H
#define UTIL_MISC_H

/**
 * \file
 * \author Lennart Braun <3braun@informatik.uni-hamburg.de>
 */

#include "neuralnet/neuralnet.h"
#include "go/player.h"

/**
 * \brief Checks whether the networks in a set have the right layout.
 * \param set Set of neural networks.
 * \param board_size Size of the board to play with.
 * \param ver network layout version
 * \return 0 if everything is ok, else != 0.
 * \pre set != NULL
 * \pre board_size > 1
 */
int check_board_size (nnet_set_t* set, size_t board_size, netver_t ver);

#endif /* UTIL_MISC_H */
