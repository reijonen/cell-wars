#include "engine.h"
#include "window.h"
#include "../game/game.h"

struct Engine engine_init()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	SDL_Window *wnd = window_create();

	return (struct Engine){
		.wnd = wnd};
}

void engine_run(struct Engine *engine, struct Game *game)
{
	bool should_exit = false;
	while (!should_exit)
	{
		SDL_Event evnt;

		while (SDL_PollEvent(&evnt))
		{
			if (evnt.type == SDL_EVENT_QUIT)
			{
				should_exit = true;
			}
		}
	}
}

void engine_terminate(struct Engine *engine)
{
	SDL_DestroyWindow(engine->wnd);
	SDL_Quit();
}