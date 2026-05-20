#include "engine/engine.h"
#include "game/game.h"
#include "engine/camera.h"

typedef struct Vertex
{
	float x;
	float y;
} Vertex;

// Vertex verts[] = {
// 	{640, 540}, // (640 / 1280 * 2) - 1 = 0 ; (540 / 720 * 2) - 1 = 0.5
// 	{960, 180}, // (960 / 1280 * 2) - 1 = 0.5 ; (180 / 720 * 2) - 1 = -0.5
// 	{320, 180}, // (320 / 1280 * 2) - 1 ; (180 / 720 * 2) - 1 = -0.5
// };

void generate_quad(Vertex *verts, Vec2 top_left, unsigned size)
{
	verts[0].x = top_left.x;
	verts[0].y = top_left.y;
	verts[1].x = top_left.x + size;
	verts[1].y = top_left.y;
	verts[2].x = top_left.x;
	verts[2].y = top_left.y - size;
	verts[3].x = top_left.x + size;
	verts[3].y = top_left.y;
	verts[4].x = top_left.x + size;
	verts[4].y = top_left.y - size;
	verts[5].x = top_left.x;
	verts[5].y = top_left.y - size;
}

int main()
{
	Vertex verts[18] = {0};
	generate_quad(verts, (Vec2){100, 100}, 50);
	generate_quad(verts + 6, (Vec2){300, 200}, 50);
	generate_quad(verts + 12, (Vec2){500, 300}, 50);

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