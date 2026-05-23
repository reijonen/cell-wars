#pragma once

#include <SDL3/SDL.h>

#include "renderer.h"

typedef struct App
{
	void *state;
	void (*update)(void *state, SDL_Event *event, unsigned int event_count, double delta_time);
	void (*render)(void *state);
} App;

void engine_release();

void engine_init(unsigned window_width, unsigned window_height);
void engine_run(App *app);