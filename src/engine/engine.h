#pragma once

#include <SDL3/SDL.h>

#include "../game/game.h"

struct Engine
{
	SDL_Window *wnd;
};

struct Engine engine_init();
void engine_run(struct Engine *engine, struct Game *game);
void engine_terminate(struct Engine *engine);