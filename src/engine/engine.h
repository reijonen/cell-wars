#pragma once

#include <SDL3/SDL.h>

#include "renderer.h"

typedef struct Engine
{
	SDL_Window *wnd;
	Renderer renderer;
} Engine;

typedef struct App
{
	void *state;
	void (*update)(void *state, SDL_Event *event, unsigned int event_count, double delta_time);
	void (*render)(void *state);
} App;

void engine_release(Engine *engine);

Engine engine_new();
void engine_run(Engine *engine, App *app);