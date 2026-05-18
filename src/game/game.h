#pragma once

#include "../engine/input.h"
#include "base.h"

typedef struct Game
{
	Base bases[1];
} Game;

Game game_new();
void game_update(void *state, SDL_Event *events, unsigned int event_count, double delta_time);
void game_render(void *state);
void game_release(Game *game);