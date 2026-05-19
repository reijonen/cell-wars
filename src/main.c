#include "engine/engine.h"
#include "game/game.h"
#include "engine/camera.h"

typedef struct Vertex
{
	float x;
	float y;
} Vertex;

// {0.0, 0.5},
// {0.5, -0.5},
// {-0.5, -0.5},

Vertex verts[] = {
	{640, 540}, // (640 / 1280 * 2) - 1 = 0 ; (540 / 720 * 2) - 1 = 0.5
	{960, 180}, // (960 / 1280 * 2) - 1 = 0.5 ; (180 / 720 * 2) - 1 = -0.5
	{320, 180}, // (320 / 1280 * 2) - 1 ; (180 / 720 * 2) - 1 = -0.5
};

int main()
{
	engine_init();

	renderer_shader_new("./shader.metal");
	renderer_buffer_new(verts, sizeof(Vertex), sizeof(verts) / sizeof(Vertex));
	renderer_pipeline_new(sizeof(Vertex), TRIANGLE_PRIMITIVE);

	Game game = game_new();

	App app = {
		.state = &game,
		.update = game_update,
		.render = game_render};

	engine_run(&app);

	game_release(&game);
	engine_release();

	return 0;
}