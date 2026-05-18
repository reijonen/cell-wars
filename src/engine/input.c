#include <SDL3/SDL.h>

#include "input.h"

unsigned int process_input(bool *should_exit, SDL_Event *events)
{
	unsigned int event_count = 0;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{

		switch (e.type)
		{
		case SDL_EVENT_QUIT:
			*should_exit = true;
			// TODO: breakkaako switch break myös loopin? entä jos tekee näin 2 kertaa?
			break;
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		case SDL_EVENT_MOUSE_MOTION:
		{
			events[event_count] = e;
			event_count++;
		}
			continue;
		}
	}

	return event_count;
}