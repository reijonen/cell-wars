#include "renderer.h"
#include "engine.h"
#include "window.h"
#include "input.h"

static SDL_Window *wnd = NULL;

void engine_init()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	wnd = window_new();

	renderer_init(wnd);
}

void engine_run(App *app)
{
	SDL_Event events[1024] = {0};

	bool should_exit = false;
	while (!should_exit)
	{
		int event_count = process_input(&should_exit, events);

		// TODO: add delta time
		app->update(app->state, events, event_count, 0.0);

		renderer_begin_frame();
		app->render(app->state);
		renderer_end_frame();
	}
}

void engine_release()
{
	renderer_release();
	window_release(wnd);
	SDL_Quit();
}