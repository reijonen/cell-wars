#include "engine/engine.h"
#include "game/game.h"

int main()
{
	Engine engine = engine_new();

	renderer_shader_new(&engine.renderer, "./shader.metal", VERTEX_SHADER);
	// renderer_shader_new(&engine.renderer, "./frag.metal", FRAGMENT_SHADER);
	renderer_pipeline_new(&engine.renderer, engine.wnd, TRIANGLE_PRIMITIVE);

	Game game = game_new();

	App app = {
		.state = &game,
		.update = game_update,
		.render = game_render};

	engine_run(&engine, &app);

	game_release(&game);
	engine_release(&engine);

	return 0;
}