#include "go/game.h"
#include "util/scheduler.h"
#include "util/util.h"

queued_game_t* init_queue_element(game_t* game)
{
	queued_game_t* element = SAFE_MALLOC(sizeof(queued_game_t));
	
	element->priority = 0;
	element->game = game;

	return element;
}

game_queue_t* init_queue()
{
	game_queue_t* queue = SAFE_MALLOC(sizeof(game_queue_t));

	queue->first = NULL;
	queue->last = NULL;
	queue->game_count = 0;

	return queue;
}

void add_game(game_queue_t* queue, queued_game_t* game)
{
	if(queue->first != NULL)
	{
		queue->last->next_game = game;
		queue->last = game;
	}
	else
	{
		queue->first = game;
		queue->last = queue->first;
	}	
	
}

game_t* select_next(game_queue_t* queue)
{
	queued_game_t* ret =  queue->first;
	queue->first = queue->first->next_game;

	return ret->game;
}

