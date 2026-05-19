#include <SDL3/SDL.h>

#include "game.h"
#include "base.h"

// TODO: tmp
#include "engine/renderer.h"

const unsigned WORLD_WIDTH = 1280;
const unsigned WORLD_HEIGHT = 720;

Game game_new()
{
	Game game;

	game.camera = camera_init(1280, 720);

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

	renderer_update_uniform(game->camera.projection, sizeof(game->camera.projection));
}

void game_render(void *state)
{
	Game *game = (Game *)state;
}

void game_release(Game *game)
{
}