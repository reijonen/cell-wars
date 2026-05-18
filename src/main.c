#include "engine/engine.h"
#include "game/game.h"

typedef struct Vertex
{
	float x;
	float y;
} Vertex;

Vertex verts[] = {
	{0.0, 0.5},
	{0.5, -0.5},
	{-0.5, -0.5},
};

int main()
{
	Engine engine = engine_new();

	renderer_shader_new(&engine.renderer, "./shader.metal");
	renderer_buffer_new(&engine.renderer, verts, sizeof(Vertex), sizeof(verts) / sizeof(Vertex));
	renderer_pipeline_new(&engine.renderer, engine.wnd, sizeof(Vertex), TRIANGLE_PRIMITIVE);

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