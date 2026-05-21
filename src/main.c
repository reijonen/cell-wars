#include "engine/engine.h"
#include "game/game.h"
#include "engine/camera.h"

typedef struct Color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Color;

typedef struct Vertex
{
	Vec2 pos;
	Color color;
} Vertex;

void generate_triangle(Vertex *verts, Vec2 top_center, unsigned size)
{
	verts[0].pos.x = top_center.x;
	verts[0].pos.y = top_center.y;

	verts[1].pos.x = top_center.x + size / 2;
	verts[1].pos.y = top_center.y - size;

	verts[2].pos.x = top_center.x - size / 2;
	verts[2].pos.y = top_center.y - size;
}

void generate_quad(Vertex *v, Vec2 top_left, unsigned size, Faction faction)
{
	Color color = {255};
	color.a = 255;
	if (faction == RED_FACTION)
	{
		color.r = 255;
		color.g = 0;
		color.b = 0;
	}
	else if (faction == BLUE_FACTION)
	{
		color.r = 0;
		color.g = 0;
		color.b = 255;
	}
	else
	{
		color.r = 255;
		color.g = 255;
		color.b = 255;
	}

	v[0].pos.x = top_left.x;
	v[0].pos.y = top_left.y;
	v[0].color = color;

	v[1].pos.x = top_left.x + size;
	v[1].pos.y = top_left.y;
	v[1].color = color;

	v[2].pos.x = top_left.x;
	v[2].pos.y = top_left.y - size;
	v[2].color = color;

	v[3].pos.x = top_left.x + size;
	v[3].pos.y = top_left.y;
	v[3].color = color;

	v[4].pos.x = top_left.x + size;
	v[4].pos.y = top_left.y - size;
	v[4].color = color;

	v[5].pos.x = top_left.x;
	v[5].pos.y = top_left.y - size;
	v[5].color = color;
}

int main()
{
	Base blue = base_new(BLUE_FACTION, (Vec2){100, 100});
	Base red = base_new(RED_FACTION, (Vec2){300, 200});
	Base neutral = base_new(NEUTRAL_FACTION, (Vec2){500, 300});

	Vertex base_vertices[18] = {0};
	generate_quad(base_vertices, blue.pos, blue.size, BLUE_FACTION);
	generate_quad(base_vertices + 6, red.pos, red.size, RED_FACTION);
	generate_quad(base_vertices + 12, neutral.pos, neutral.size, NEUTRAL_FACTION);

	engine_init();

	renderer_shader_new("./shader.metal");
	renderer_buffer_new(base_vertices, sizeof(Vertex), sizeof(base_vertices) / sizeof(Vertex));
	renderer_pipeline_new(sizeof(Vertex), sizeof(Vec2), TRIANGLE_PRIMITIVE);

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