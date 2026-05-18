#include <SDL3/SDL.h>

#include "game.h"
#include "base.h"

Game game_new()
{
	Game game;

	return game;
}

void game_update(void *state, SDL_Event *events, unsigned int event_count, double delta_time)
{
	Game *game = (Game *)state;

	for (unsigned int i = 0; i < event_count; i++)
	{
		switch (events[i].type)
		{
		case SDL_EVENT_MOUSE_BUTTON_DOWN:

			continue;

		case SDL_EVENT_MOUSE_BUTTON_UP:

			continue;

		case SDL_EVENT_MOUSE_MOTION:

			continue;
		}
	}

	for (unsigned i = 0; i < (sizeof(game->bases) / sizeof(Base)); i++)
	{
		base_update(&game->bases[i]);
	}
}

void game_render(void *state)
{
	Game *game = (Game *)state;
}

void game_release(Game *game)
{
}