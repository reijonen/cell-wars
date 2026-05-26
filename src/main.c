#include "engine/engine.h"
#include "game/game.h"
#include "game/game_renderer.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define BASE_SIZE 50

int main()
{
	/*
				| 4 |
		(5)/     |(6)    \(7)
	 | 1 | -(3) | 2 | (4)- | 3 |
		(0)\     |(1)    /(2)
				| 0 |
	*/

	Game game = game_new(WINDOW_WIDTH, WINDOW_HEIGHT, BASE_SIZE);
	GameRenderer game_renderer = {0};

	engine_init(WINDOW_WIDTH, WINDOW_HEIGHT);
	game_renderer_init(&game_renderer, &game);

	App app = {
		.state = &game,
		.update = game_update,
		.render = game_render};

	engine_run(&app);

	game_renderer_release(&game_renderer);
	game_release(&game);
	engine_release();

	return 0;
}
