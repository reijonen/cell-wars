#include "renderer.h"
#include "engine.h"
#include "window.h"
#include "input.h"

static SDL_Window *wnd = NULL;

void engine_init(unsigned window_width, unsigned window_height)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	wnd = window_new(window_width, window_height);

	renderer_init(wnd);
}

void engine_run(App *app)
{
	SDL_Event events[1024] = {0};
	Uint64 previous_ticks = SDL_GetTicks();

	bool should_exit = false;
	while (!should_exit)
	{
		int event_count = process_input(&should_exit, events);
		Uint64 current_ticks = SDL_GetTicks();
		double delta_time = (double)(current_ticks - previous_ticks) / 1000.0;
		previous_ticks = current_ticks;

		app->update(app->state, events, event_count, delta_time);

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
