#include "renderer.h"
#include "engine.h"
#include "window.h"
#include "input.h"

Engine engine_new()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	SDL_Window *wnd = window_new();
	Renderer renderer = renderer_new(wnd);

	return (Engine){
		.wnd = wnd,
		.renderer = renderer};
}

void engine_run(Engine *engine, App *app)
{
	SDL_Event events[1024] = {0};

	bool should_exit = false;
	while (!should_exit)
	{
		int event_count = process_input(&should_exit, events);

		// TODO: add delta time
		app->update(app->state, events, event_count, 0.0);

		renderer_begin_frame(&engine->renderer, engine->wnd);
		app->render(app->state);
		renderer_end_frame(&engine->renderer);
	}
}

void engine_release(Engine *engine)
{
	renderer_release(&engine->renderer, engine->wnd);
	window_release(engine->wnd);
	SDL_Quit();
}