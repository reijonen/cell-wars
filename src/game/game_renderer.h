#pragma once

#include <stdbool.h>

#include "game.h"

typedef struct GameRenderer
{
	bool initialized;
} GameRenderer;

void game_renderer_init(GameRenderer *renderer, Game *game);
void game_renderer_release(GameRenderer *renderer);
