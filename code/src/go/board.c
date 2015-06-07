#include "board.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void board_print(board_t *board, bool complete) {
  if (complete)
    for (int i = 0; i < board->size + 2; ++i) {
      for (int j = 0; j < board->size + 2; ++j) {
        printf("%d ", board->buffer[i * (board->size + 2) + j]);
      }
      printf("\n");
    }
  else
    for (int i = 0; i < board->size; ++i) {
      for (int j = 0; j < board->size; ++j) {
        printf("%d ", board->grid[i][j]);
      }
      printf("\n");
    }
}

board_t *board_create(uint8_t size) {
  board_t *board = NULL;

  board = malloc(sizeof(board_t));
  if (board == NULL) {
    fprintf(stderr, "malloc() failed in file %s at line # %d", __FILE__,
            __LINE__);
    exit(EXIT_FAILURE);
  }

  board->buffer = calloc((size_t)(size * size), sizeof(uint8_t));
  if (board->buffer == NULL) {
    fprintf(stderr, "calloc() failed in file %s at line # %d", __FILE__,
            __LINE__);
    free(board);
    exit(EXIT_FAILURE);
  }

  board->grid = calloc((size_t)size, sizeof(int8_t *));
  if (board->grid == NULL) {
    fprintf(stderr, "calloc() failed in file %s at line # %d", __FILE__,
            __LINE__);
    free(board);
    free(board->buffer);
    exit(EXIT_FAILURE);
  }

  for (uint8_t i = 0; i < size; ++i) {
    board->grid[i] = board->buffer + i * size;
  }

  board->size = size;
  board->turn = c_black;
  board->black_captured = 0;
  board->white_captured = 0;

  return board;
}

void board_destroy(board_t *board) {
  free(board->grid);
  free(board->buffer);
  free(board);
}

pos_state_t board_position_state(const board_t *board, uint8_t x, uint8_t y) {
  return board->grid[x][y];
}

bool board_legal_placement(const board_t *board, uint8_t x, uint8_t y,
                           color_t color) {
  if (x >= board->size || y >= board->size)
    return false;
  if (board->turn != color)
    return false;
  if (board->grid[x][y] != ps_empty)
    return false;
  if (board_num_liberties(board, board_get_group(board, x, y)) == 0)
    return false;
  return true;
}

void board_place(board_t *board, uint8_t x, uint8_t y) {
  board->grid[x][y] = (pos_state_t)board->turn;
  board_capture(board, x, y);
  board->turn = (board->turn == c_black) ? c_white : c_black;
}

void board_pass(board_t *board) {
  board->turn = (board->turn == c_black) ? c_white : c_black;
}

uint16_t board_num_liberties(const board_t *board, int *group) {
  uint16_t liberties = 0;
  uint8_t a, b, left, right, top, bottom;

  for (int i = 1; i <= group[0]; i += 2) {
    a = group[i];
    b = group[i + 1];
    left = a - 1;
    right = a + 1;
    top = b - 1;
    bottom = b + 1;

    if (left < board->size &&
        board_position_state(board, left, b) == ps_empty)
      liberties += 1;
    if (right < board->size &&
        board_position_state(board, right, b) == ps_empty)
      liberties += 1;
    if (top < board->size &&
        board_position_state(board, a, top) == ps_empty)
      liberties += 1;
    if (bottom < board->size &&
        board_position_state(board, a, bottom) == ps_empty)
      liberties += 1;
  }

  return liberties;
}

int *board_get_group(const board_t *board, uint8_t x, uint8_t y) {

  // Finds out which color to look for
  pos_state_t state = board_position_state(board, x, y);

  // The first stone is always the one on the position given, so the intial size
  // is one.
  int size = 1;
  int *group = calloc(120, sizeof(int));
	group[0] = size;
  group[1] = x;
  group[2] = y;
  board->grid[x][y] = ps_marked;
  int current = 1;
  int index = 3;

  // Alway goes left, right, up or down from every stone until the next one is
  // either marked or of a different color
  while (current < index) 
  {
    // x and y coordinates of the current stone
    uint8_t a = group[current];
    uint8_t b = group[current + 1];

    uint8_t left = a - 1;
    uint8_t right = a + 1;
    uint8_t top = b - 1;
    uint8_t bottom = b + 1;
    uint8_t neighbours[4] = {left, right, top, bottom};

    for (int i = 0; i < 2; ++i) {
      while (neighbours[i] < board->size && neighbours[i] >= 0 &&
             board_position_state(board, neighbours[i], y) == state) 
      {
        if (board_position_state(board, neighbours[i], y) != ps_marked) 
	{
          group[index] = neighbours[i];
          group[index + 1] = y;
          board->grid[neighbours[i]][y] = ps_marked;
          index+=2;
          ++size;
        }
        neighbours[i] = neighbours[i+2] + (int) pow(-1, i);
      }

      while (neighbours[i+2] < board->size && neighbours[i+2] >= 0 &&
             board_position_state(board, x, neighbours[i+2]) == state) 
      {
        if (board_position_state(board, x, neighbours[i+2]) != ps_marked) 
	{
          group[index] = x;
          group[index + 1] = neighbours[i+2];
          board->grid[x][neighbours[i+2]] = ps_marked;
          index+=2;
          ++size;
        }
        
	neighbours[i+2] = neighbours[i+2] + (int) pow(-1, i);
      }
    }

    current += 2;
  }

  group[0] = size;

  // Resets the marked states
  for (int j = 1; j <= size; j += 2) {
    board->grid[group[j]][group[j + 1]] = state;
  }

  return group;
}

void board_capture(board_t *board, uint8_t x, uint8_t y) {
  // Determines starting position and neighbours to check
  pos_state_t state = board_position_state(board, x, y);
  int stones_captured = 0;
  uint8_t left = x - 1;
  uint8_t right = x + 1;
  uint8_t top = y - 1;
  uint8_t bottom = y + 1;
  uint8_t neighbours[4] = {left, right, top, bottom};

  // Gets each neighbour and checks their number of liberties; if it's zero,
  // they're deleted

  int *current;

  for (int i = 0; i < 4; ++i) {
    if (neighbours[i] >= 0 && neighbours[i] < board->size) {
      current = board_get_group(board, neighbours[i], y);

      if (board_num_liberties(board, current) == 0) {

        for (int j = 1; j <= current[0]; ++j) {
          board->grid[current[j]][current[j + 1]] = ps_empty;
          ++stones_captured;
        }
      }
    }
  }

  // Updates score
  if (state == ps_white)
    board->white_captured += stones_captured;
  else
    board->black_captured += stones_captured;
}

int board_score(const board_t *board, uint8_t size, uint8_t komi) {
  int final_score;
  int *groups_white = malloc(60*sizeof(int)); 
  int *groups_black = malloc(60*sizeof(int));  
  int size_white = 0;
  int size_black = 0;
  uint8_t ind_white = 1;
  uint8_t ind_black = 1;

  for (int i = 0; i < board->size; i++) {
    for (int j = 0; j < board->size; j++) {
      if (board_position_state(board, i, j) == ps_empty) {
        int *group = board_get_group(board, i, j);
        pos_state_t state;

        if (i > 0) {
          int a = i - 1;
          state = board_position_state(board, a, j);
        } else {
          int a = i + 1;
          state = board_position_state(board, a, j);
        }

        if (state == ps_white) {
          groups_white[ind_white] = group[0];
          ++size_white;
        } else {
          groups_black[ind_black] = group[0];
          ++size_white;
        }
      }
    }
  }

  final_score = (int)board_score_black(board, groups_black) -
                (int)board_score_white(board, groups_white) - komi;

  return final_score;
}

uint8_t board_score_white(const board_t *board, int *groups_white) {
  uint8_t score = board->white_captured;

  for (int i = 0; i <= groups_white[0]; i++) {
    score += (int)groups_white[i];
  }

  return score;
}

uint8_t board_score_black(const board_t *board, int *groups_black) {
  uint8_t score = board->black_captured;

  for (int i = 0; i <= groups_black[0]; i++) {
    score += (int)groups_black[i];
  }

  return score;
}
