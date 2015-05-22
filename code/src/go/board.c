#include "board.h"

#include <stddef.h>


board_t* board_create (uint8_t size)
{
    return NULL;
}

void board_destroy (board_t* board)
{
}

pos_state_t board_position_state (board_t* board, uint8_t x, uint8_t y)
{
    return 0;
}

bool board_legal_placement (const board_t* board, uint8_t x, uint8_t y, color_t color)
{
    return false;
}

void board_place (board_t* board, uint8_t x, uint8_t y, color_t color)
{
}

void board_pass (board_t* board, color_t color)
{
}
