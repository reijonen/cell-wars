#include "engine/engine.h"
#include "game/game.h"

int main()
{
	struct Engine engine = engine_init();
	struct Game game = game_init();

	engine_run(&engine, &game);

	game_terminate(&game);
	engine_terminate(&engine);

	return 0;
}