#include "go/game.h"

/*
 * \brief A simple scheduler for games
 */

// Datatype for queue element
typedef struct queued_game_t
{
    size_t p1;
    size_t p2;
    int priority;
    struct queued_game_t* next_game;
} queued_game_t;

typedef struct game_queue_t
{
    queued_game_t* first;
    queued_game_t* last;
    bool empty;
} game_queue_t;

// Init function for one queue element
queued_game_t* init_queue_element (size_t p1, size_t p2);

// Init function for queue
game_queue_t* init_queue (size_t max_size);

// Add a game
void add_game (game_queue_t* queue, queued_game_t* game);

// Returns next game; here the scheduling principle is applied
queued_game_t* select_next (game_queue_t* queue);
