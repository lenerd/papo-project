#include "board.h"

#include "util/util.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const size_t invalid_1d = SIZE_MAX;

static void set_ko (board_t* board, size_t placed, size_t captured);
static void clean_ko (board_t* board);

void board_print (const board_t* board, FILE* file)
{
    fputc ('\n ', file);
    for (size_t y = 0; y < board->size; ++y){
        fputc (' ', file);
        fputc (y + '0', file);
    }
    fputc (' ', file);
    fputc ('\n', file);
    
    for (size_t x = 0; x < board->size; ++x)
    {
        fputc (x + '0', file);
        for (size_t y = 0; y < board->size; ++y)
        {
            pos_state_t ps = board->grid[x][y];
            char symbol;
            switch (ps)
            {
                case ps_black:
                    symbol = 'B';
                    break;
                case ps_white:
                    symbol = 'W';
                    break;
                case ps_empty:
                    symbol = '+';
                    break;
                default:
                    symbol = '?';
                    break;
            }
            fputc (' ', file);
            fputc (symbol, file);
        }
        fputc (' ', file);
        fputc (x + '0', file);
        fputc ('\n\n', file);
    }

    fputc (' ', file);
    for (size_t y = 0; y < board->size; ++y){
        fputc (' ', file);
        fputc (y + '0', file);
    }
    fputc (' ', file);
    fputc ('\n', file);
}

board_t* board_create (size_t size)
{
    board_t* board = NULL;

    board = SAFE_MALLOC (sizeof (board_t));

    board->size = size;
    board->buf_size = size * size;
    board->turn = c_black;
    board->black_captured = 0;
    board->white_captured = 0;
    board->ko = false;

    board->buffer = SAFE_CALLOC (board->buf_size, sizeof (uint8_t));

    board->grid = SAFE_CALLOC (size, sizeof (int8_t*));

    board->group_next =
        SAFE_MALLOC (board->buf_size * sizeof (size_t));
    memset (board->group_next, 0xFF, board->buf_size * sizeof (size_t));

    board->group_id =
        SAFE_MALLOC (board->buf_size * sizeof (size_t));
    memset (board->group_id, 0xFF, board->buf_size * sizeof (size_t));

    board->group_liberties =
        SAFE_CALLOC (board->buf_size, sizeof (uint16_t));

    board->mark_buf = SAFE_CALLOC (board->buf_size, sizeof (uint8_t));

    for (size_t i = 0; i < size; ++i)
    {
        board->grid[i] = board->buffer + i * size;
    }

    return board;
}

void board_destroy (board_t* board)
{
    free (board->grid);
    free (board->buffer);
    free (board->group_next);
    free (board->group_id);
    free (board->group_liberties);
    free (board->mark_buf);
    free (board);
}

pos_state_t board_position_state (const board_t* board, size_t x, size_t y)
{
    return board->grid[x][y];
}

bool board_legal_placement (const board_t* board, size_t x, size_t y,
                            color_t color)
{
    if (x >= board->size || y >= board->size)
        return false;
    if (board->grid[x][y] != ps_empty)
        return false;
    if (board_test_suicide (board, x, y, color))
        return false;
    if (board_test_ko (board, x, y))
        return false;
    return true;
}

void board_place_color (board_t* board, size_t x, size_t y, color_t color)
{
    assert (board_legal_placement (board, x, y, color));

    clean_ko (board);

    board->grid[x][y] = (pos_state_t) color;
    size_t pos = board_2d_to_1d (board, x, y);
    board->group_next[pos] = pos;
    board->group_id[pos] = pos;

    /* update groups */
    board_join_groups (board, x, y);
    /* capture stones */
    board_capture (board, x, y);
}

void board_place (board_t* board, size_t x, size_t y)
{
    assert (board_legal_placement (board, x, y, board->turn));

    board_place_color (board, x, y, board->turn);
    board->turn = (board->turn == c_black) ? c_white : c_black;
}

void board_pass (board_t* board)
{
    assert (board != NULL);

    board->turn = (board->turn == c_black) ? c_white : c_black;
    clean_ko (board);
}

uint16_t board_num_liberties (const board_t* board, size_t group)
{
    assert (board != NULL);
    assert (group != invalid_1d);
    assert (group < board->buf_size);
    assert (board->buffer[group] == ps_black ||
            board->buffer[group] == ps_white);

    return board->group_liberties[group];
}

bool board_test_suicide (const board_t* board, size_t x, size_t y,
                         color_t color)
{
    assert (board != NULL);
    assert (x < board->size);
    assert (y < board->size);

    color_t enemy = color == c_white ? c_black : c_white;
    size_t pos = board_2d_to_1d (board, x, y);
    size_t left = board_1d_left (board, pos);
    size_t right = board_1d_right (board, pos);
    size_t top = board_1d_top (board, pos);
    size_t bot = board_1d_bot (board, pos);
    size_t n[4] = {left, right, top, bot};

    bool last_lib = false;
    bool other_lib = false;
    bool last_lib_enemy = false;
    uint8_t enemy_cnt = 0;
    uint16_t libs = UINT16_MAX;

    for (size_t x = 0; x < 4; ++x)
    {
        if (n[x] == invalid_1d)
        {
            ++enemy_cnt;
            continue;
        }
        if (board->buffer[n[x]] == ps_empty)
        {
            return false;
        }
        else if (board->buffer[n[x]] == enemy)
        {
            ++enemy_cnt;
            if (board->group_liberties[board->group_id[n[x]]] == 1)
                last_lib_enemy = true;
        }
        else if (board->buffer[n[x]] == color)
        {
            libs = board->group_liberties[board->group_id[n[x]]];
            if (libs == 1)
                last_lib = true;
            else
                other_lib = true;
        }
    }

    return !other_lib && !last_lib_enemy &&
           (last_lib || (!last_lib && (enemy_cnt == 4)));
}

static void set_ko (board_t* board, size_t placed, size_t captured)
{
    assert (board != NULL);
    assert (placed < board->buf_size);
    assert (captured < board->buf_size);
    assert (placed != captured);

    board->ko_placed = placed;
    board->ko_captured = captured;
    board->ko = true;
}

static void clean_ko (board_t* board)
{
    assert (board != NULL);

    if (board->ko)
    {
        board->ko_placed = invalid_1d;
        board->ko_placed = invalid_1d;
        board->ko = false;
    }
}

bool board_test_ko (const board_t* board, size_t x, size_t y)
{
    assert (board != NULL);
    assert (x < board->size);
    assert (y < board->size);

    if (!board->ko)
        return false;

    size_t placed = board->ko_placed;
    size_t captured = board->ko_captured;

    if (board_2d_to_1d (board, x, y) != captured)
        return false;

    if (board->group_liberties[board->group_id[placed]] > 1)
        return false;

    pos_state_t ps = board->buffer[placed];

    /* group size > 1 ? */
    size_t left = board_1d_left (board, placed);
    size_t right = board_1d_right (board, placed);
    size_t top = board_1d_top (board, placed);
    size_t bot = board_1d_bot (board, placed);
    size_t n[4] = {left, right, top, bot};

    for (size_t x = 0; x < 4; ++x)
        if (n[x] != invalid_1d && board->buffer[n[x]] == ps)
            return false;

    return true;
}

size_t board_get_group_id (const board_t* board, size_t x, size_t y)
{
    assert (board != NULL);
    assert (x < board->size);
    assert (y < board->size);

    return board->group_id[board_2d_to_1d (board, x, y)];
}

uint64_t board_capture_group (board_t* board, size_t group)
{
    assert (board != NULL);
    assert (group < board->buf_size);
    assert (board->buffer[group] != ps_empty);
    assert (group == board->group_id[group]);

    uint64_t stones_captured = 0;
    size_t tmp_next;

    board->group_id[group] = invalid_1d;
    board->group_liberties[group] = 0;
    tmp_next = board->group_next[group];
    board->group_next[group] = invalid_1d;
    board->buffer[group] = ps_empty;
    ++stones_captured;
    for (size_t i = tmp_next; board->group_id[i] != invalid_1d; i = tmp_next)
    {
        tmp_next = board->group_next[i];
        board->group_id[i] = invalid_1d;
        board->group_liberties[i] = 0;
        board->group_next[i] = invalid_1d;
        board->buffer[i] = ps_empty;
        ++stones_captured;
    }
    return stones_captured;
}

uint64_t board_capture (board_t* board, size_t x, size_t y)
{
    assert (board != NULL);
    assert (x < board->size);
    assert (y < board->size);
    assert (board->grid[x][y] == ps_black || board->grid[x][y] == ps_white);

    color_t state = (color_t) board_position_state (board, x, y);
    color_t enemy = state == c_white ? c_black : c_white;
    uint64_t stones_captured = 0;
    size_t pos = board_2d_to_1d (board, x, y);
    size_t left = board_1d_left (board, pos);
    size_t right = board_1d_right (board, pos);
    size_t top = board_1d_top (board, pos);
    size_t bot = board_1d_bot (board, pos);
    size_t group;
    size_t ko_captured;

    size_t n[4] = {left, right, top, bot};
    for (size_t x = 0; x < 4; ++x)
    {
        if (n[x] != invalid_1d && board->buffer[n[x]] == enemy)
        {
            group = board->group_id[n[x]];
            if (board_num_liberties (board, group) == 0)
            {
                stones_captured += board_capture_group (board, group);
                ko_captured = n[x];
            }
        }
    }

    /* update liberties */
    if (stones_captured)
    {
        for (size_t i = 0; i < board->buf_size; ++i)
        {
            if (board->group_id[i] == i)
            {
                position_t pos = board_1d_to_2d (board, i);
                board_calc_liberties (board, pos.x, pos.y);
            }
        }
    }

    /* Updates score */
    if (state == c_white)
        board->white_captured =
            (uint16_t)(board->white_captured + stones_captured);
    else
        board->black_captured =
            (uint16_t)(board->black_captured + stones_captured);

    /* set ko */
    if (stones_captured == 1)
    {
        set_ko (board, pos, ko_captured);
    }

    return stones_captured;
}

uint16_t flood_fill (const board_t* board, size_t index, uint8_t* owner)
{
    uint16_t cnt = 1;
    size_t next[4];
    board->mark_buf[index] = ps_marked;
    *owner |= board->buffer[index];

    next[0] = board_1d_top (board, index);
    next[1] = board_1d_right (board, index);
    next[2] = board_1d_bot (board, index);
    next[3] = board_1d_left (board, index);
    for (size_t i = 0; i < 4; ++i)
    {
        if (next[i] == invalid_1d)
            continue;
        if (board->mark_buf[next[i]] & ps_marked)
            continue;
        if (board->buffer[next[i]])
            *owner |= board->buffer[next[i]];
        else
        {
            cnt = (uint16_t)(cnt + flood_fill (board, next[i], owner));
        }
    }

    return cnt;
}

int board_score (const board_t* board)
{
    int score = 0;
    int tmp;
    uint8_t owner;
    for (size_t index = 0; index < board->buf_size; ++index)
    {
        if (board->buffer[index] == ps_empty &&
            board->mark_buf[index] != ps_marked)
        {
            owner = ps_empty;
            tmp = flood_fill (board, index, &owner);
            if (!(owner & ~ps_black))
                score += tmp;
            else if (!(owner & ~ps_white))
                score -= tmp;
        }
    }
    memset (board->mark_buf, 0, board->buf_size);
    score += board->black_captured - board->white_captured;
    return score;
}

size_t board_2d_to_1d (const board_t* board, size_t x, size_t y)
{
    assert (x < board->size);
    assert (y < board->size);
    return (x * board->size + y);
}

position_t board_1d_to_2d (const board_t* board, size_t z)
{
    position_t pos;
    pos.x = z / board->size;
    pos.y = z % board->size;
    return pos;
}

size_t board_join_groups (board_t* board, size_t x, size_t y)
{
    assert (board != NULL);
    assert (x < board->size);
    assert (y < board->size);
    assert (board->grid[x][y] == ps_black || board->grid[x][y] == ps_white);

    color_t turn = board->grid[x][y];

    size_t pos, left, right, top, bot;

    pos = board_2d_to_1d (board, x, y);

    left = board_1d_left (board, pos);
    right = board_1d_right (board, pos);
    top = board_1d_top (board, pos);
    bot = board_1d_bot (board, pos);
    size_t n[4] = {left, right, top, bot};

    size_t min_id = pos;

    /* merge groups */
    for (size_t x = 0; x < 4; ++x)
    {
        /* update if
         * * position is valid
         * * it's occupied by a stone of the same color
         * * this stone is in another group
         */
        if (n[x] != invalid_1d && board->buffer[n[x]] == turn &&
            board->group_id[n[x]] != board->group_id[pos])
        {
            size_t tmp;

            /* merge group cycles */
            tmp = board->group_next[n[x]];
            board->group_next[n[x]] = board->group_next[pos];
            board->group_next[pos] = tmp;

            /* update group id */
            min_id = (board->group_id[n[x]] < min_id) ? board->group_id[n[x]] :
                                                        min_id;

            board->group_id[pos] = min_id;
            for (size_t i = board->group_next[pos]; i != pos;
                 i = board->group_next[i])
                board->group_id[i] = min_id;
        }
    }

    /* set group id */
    board->group_id[pos] = min_id;

    /* calculate liberties of merged group */
    board_calc_liberties (board, x, y);

    /* update liberties of neighboured enemy groups */
    pos_state_t enemy = ((pos_state_t) turn == ps_black) ? ps_white : ps_black;
    if (x > 0 && board->grid[x - 1][y] == enemy)
        board_calc_liberties (board, x - 1, y);
    if (x < board->size - 1 && board->grid[x + 1][y] == enemy)
        board_calc_liberties (board, x + 1, y);
    if (y > 0 && board->grid[x][y - 1] == enemy)
        board_calc_liberties (board, x, y - 1);
    if (y < board->size - 1 && board->grid[x][y + 1] == enemy)
        board_calc_liberties (board, x, y + 1);

    return min_id;
}

uint16_t board_calc_liberties (board_t* board, size_t x, size_t y)
{
    assert (board != NULL);
    assert (x < board->size);
    assert (y < board->size);
    assert (board->grid[x][y] != ps_empty);

    size_t pos = board_2d_to_1d (board, x, y);
    uint16_t lib = 0;
    size_t n[4];

    n[0] = board_1d_left (board, pos);
    n[1] = board_1d_right (board, pos);
    n[2] = board_1d_top (board, pos);
    n[3] = board_1d_bot (board, pos);
    for (size_t x = 0; x < 4; ++x)
    {
        if (n[x] != invalid_1d && board->buffer[n[x]] == (uint8_t) ps_empty &&
            board->mark_buf[n[x]] != (uint8_t) ps_marked)
        {
            board->mark_buf[n[x]] = ps_marked;
            ++lib;
        }
    }
    for (size_t i = board->group_next[pos]; i != pos;
         i = board->group_next[i])
    {
        n[0] = board_1d_left (board, i);
        n[1] = board_1d_right (board, i);
        n[2] = board_1d_top (board, i);
        n[3] = board_1d_bot (board, i);
        for (uint8_t x = 0; x < 4; ++x)
        {
            if (n[x] != invalid_1d &&
                board->buffer[n[x]] == (uint8_t) ps_empty &&
                board->mark_buf[n[x]] != (uint8_t) ps_marked)
            {
                board->mark_buf[n[x]] = ps_marked;
                ++lib;
            }
        }
    }

    memset (board->mark_buf, 0, board->buf_size);
    board->group_liberties[board->group_id[pos]] = lib;
    return lib;
}

size_t board_1d_left (const board_t* board, size_t pos)
{
    assert (board != NULL);
    assert (pos < board->buf_size);
    if (pos % board->size == 0)
        return invalid_1d;
    return pos - 1;
}

size_t board_1d_right (const board_t* board, size_t pos)
{
    assert (board != NULL);
    assert (pos < board->buf_size);
    if (pos % board->size == board->size - 1)
        return invalid_1d;
    return pos + 1;
}

size_t board_1d_top (const board_t* board, size_t pos)
{
    assert (board != NULL);
    assert (pos < board->buf_size);
    if (pos < board->size)
        return invalid_1d;
    return pos - board->size;
}

size_t board_1d_bot (const board_t* board, size_t pos)
{
    assert (board != NULL);
    assert (pos < board->buf_size);
    if (pos >= board->buf_size - board->size)
        return invalid_1d;
    return pos + board->size;
}
