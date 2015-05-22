#include "board.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

void board_print (board_t* board, bool complete)
{
    if (complete)
        for (int i = 0; i < board->size + 2; ++i)
        {
            for (int j = 0; j < board->size + 2; ++j)
            {
                printf("%d ", board->buffer[i * (board->size + 2) + j]);
            }
            printf("\n");
        }
    else
        for (int i = 0; i < board->size; ++i)
        {
            for (int j = 0; j < board->size; ++j)
            {
                printf("%d ", board->grid[i][j]);
            }
            printf("\n");
        }
}

board_t* board_create (uint8_t size)
{
    board_t* board = NULL;

    board = malloc(sizeof(board_t));
    if (board == NULL)
    {
        fprintf(stderr, "memory allocation failed\n");
        return NULL;
    }

    board->buffer = calloc((size_t)((size + 2) * (size + 2)), sizeof(int8_t));
    if (board->buffer == NULL)
    {
        fprintf(stderr, "memory allocation failed\n");
        free(board);
        return NULL;
    }

    board->grid = calloc((size_t)(size + 2), sizeof(int8_t*));
    if (board->grid == NULL)
    {
        fprintf(stderr, "memory allocation failed\n");
        free(board);
        free(board->buffer);
        return NULL;
    }

    for (uint8_t i = 0; i < size + 2; ++i)
    {
        board->buffer[i] = ps_illegal;
        board->buffer[(size + 1) * (size + 2) + i] = ps_illegal;
    }
    for (uint8_t i = 1; i < size + 1; ++i)
    {
        board->buffer[i * (size + 2)] = ps_illegal;
        board->buffer[i * (size + 2) + (size + 1)] = ps_illegal;
    }

    for (uint8_t i = 0; i < size + 2; ++i)
    {
        board->grid[i] = board->buffer + 1 + (i) * (size + 2);
    }
    board->grid = &board->grid[1];

    board->size = size;
    board->turn = c_black;
    board->black_captured = 0;
    board->white_captured = 0;

    return board;
}

void board_destroy (board_t* board)
{
    free(&board->grid[-1]);
    free(board->buffer);
    free(board);
}

pos_state_t board_position_state (board_t* board, uint8_t x, uint8_t y)
{
    return board->grid[x][y];
}

bool board_legal_placement (const board_t* board, uint8_t x, uint8_t y, color_t color)
{
    if (x >= board->size || y >= board->size)
        return false;
    if (board->turn != color)
        return false;
    if (board->grid[x][y] != ps_empty)
        return false;
    /* TODO: check for suicide */
    return true;
}

void board_place (board_t* board, uint8_t x, uint8_t y)
{
    board->grid[x][y] = (pos_state_t)board->turn;
    /* TODO: capture */
    board->turn = (board->turn == c_black) ? c_white : c_black;
}

void board_pass (board_t* board)
{
    board->turn = (board->turn == c_black) ? c_white : c_black;
}
