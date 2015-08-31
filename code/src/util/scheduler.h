#include "go/game.h"

/*
 * \brief A simple scheduler for games
 */

//Datatype for queue element
typedef struct queued_game_t
{
	game_t* game;
	int priority;
	struct queued_game_t* next_game;	
} queued_game_t;

typedef struct game_queue_t
{
	queued_game_t* first;
	queued_game_t* last;
	int game_count;	
} game_queue_t;

//Init function for one queue element
queued_game_t* init_queue_element(game_t* game);

//Init function for queue
game_queue_t* init_queue(int max_size);

//Add a game
void add_game(game_queue_t* queue, queued_game_t* game);

//Returns next game; here the scheduling principle is applied
game_t* select_next(game_queue_t* queue);
