#include <stdlib.h>
#include <SDL3/SDL.h>

SDL_Window *window_new(unsigned width, unsigned height)
{
	SDL_Window *wnd = SDL_CreateWindow(
		"test",
		width,
		height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_METAL);
	if (!wnd)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
		exit(1);
	}

	return wnd;
}

void window_release(SDL_Window *wnd)
{
	SDL_DestroyWindow(wnd);
}