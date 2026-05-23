#include "engine/engine.h"
#include "game/game.h"
#include "engine/camera.h"

#include "game/graph.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

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
	Color c = {0};
	c.r = 255;
	c.g = 255;
	c.b = 0;
	c.a = 255;

	verts[0].pos.x = top_center.x;
	verts[0].pos.y = top_center.y;
	verts[0].color = c;

	verts[1].pos.x = top_center.x + size / 2;
	verts[1].pos.y = top_center.y - size;
	verts[1].color = c;

	verts[2].pos.x = top_center.x - size / 2;
	verts[2].pos.y = top_center.y - size;
	verts[2].color = c;
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
		color.r = 127;
		color.g = 127;
		color.b = 127;
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

#define EDGE_COUNT 8

void generate_edge_vertices(Vertex *v, Base *bases, Node *nodes, Edge *edges)
{
	Base *base_map[BASE_COUNT] = {0};

	for (unsigned i = 0; i < BASE_COUNT; i++)
	{
		Base *base = &bases[i];
		base_map[base->nid] = base;
	}

	Color c = {0};
	c.r = 255;
	c.g = 255;
	c.b = 255;
	c.a = 255;

	unsigned vertex_idx = 0;
	for (unsigned e = 0; e < EDGE_COUNT; e++)
	{
		NID left = edges[e].endpoints[0];
		NID right = edges[e].endpoints[1];

		Base *left_base = base_map[left];
		Base *right_base = base_map[right];

		v[vertex_idx].pos.x = left_base->pos.x + left_base->size / 2;
		v[vertex_idx].pos.y = left_base->pos.y - left_base->size / 2;
		v[vertex_idx].color = c;
		vertex_idx++;

		v[vertex_idx].pos.x = right_base->pos.x + left_base->size / 2;
		v[vertex_idx].pos.y = right_base->pos.y - left_base->size / 2;
		v[vertex_idx].color = c;
		vertex_idx++;
	}
}

#define SHAPE_PIPELINE_IDX 0
#define SHAPE_BUFF_IDX 0

#define LINE_PIPELINE_IDX 1
#define LINE_BUFF_IDX 1

#define EDGE_PIPELINE_IDX 2
#define EDGE_BUFF_IDX 2

#define UNIT_PIPELINE_IDX 3
#define UNIT_BUFF_IDX 3

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

	Node nodes[BASE_COUNT];
	for (unsigned int i = 0; i < BASE_COUNT; i++)
	{
		nodes[i].id = i;
	}

	Edge edges[EDGE_COUNT];
	for (unsigned int i = 0; i < EDGE_COUNT; i++)
	{
		edges[i].id = i;
	}

	unsigned ec = 0;
	new_edge(&edges[0], &ec, 0, 1); // 0
	new_edge(&edges[1], &ec, 0, 2); // 1
	new_edge(&edges[2], &ec, 0, 3); // 2
	new_edge(&edges[3], &ec, 1, 2); // 3
	new_edge(&edges[4], &ec, 2, 3); // 4
	new_edge(&edges[5], &ec, 1, 4); // 5
	new_edge(&edges[6], &ec, 2, 4); // 6
	new_edge(&edges[7], &ec, 3, 4); // 7

	nodes[0].edges[0] = 0;
	nodes[0].edges[1] = 1;
	nodes[0].edges[2] = 2;
	nodes[0].edge_count = 3;

	nodes[1].edges[0] = 0;
	nodes[1].edges[1] = 5;
	nodes[1].edges[2] = 3;
	nodes[1].edge_count = 3;

	nodes[2].edges[0] = 1;
	nodes[2].edges[1] = 3;
	nodes[2].edges[2] = 6;
	nodes[2].edges[3] = 4;
	nodes[2].edge_count = 4;

	nodes[3].edges[0] = 2;
	nodes[3].edges[1] = 4;
	nodes[3].edges[2] = 7;
	nodes[3].edge_count = 3;

	nodes[4].edges[0] = 6;
	nodes[4].edges[1] = 5;
	nodes[4].edges[2] = 7;
	nodes[4].edge_count = 3;

	Base bases[BASE_COUNT];

	base_init(
		&bases[0],
		0,
		BLUE_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 2) - (BASE_SIZE / 2),
			200 + (BASE_SIZE / 2)},
		BASE_SIZE);

	base_init(
		&bases[4],
		4,
		RED_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 2) - (BASE_SIZE / 2),
			(WINDOW_HEIGHT - 200) + (BASE_SIZE / 2)},
		BASE_SIZE);

	base_init(
		&bases[1],
		1,
		NEUTRAL_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 3) - (BASE_SIZE / 2),
			(WINDOW_HEIGHT / 2) + (BASE_SIZE / 2)},
		BASE_SIZE);

	base_init(
		&bases[2],
		2,
		NEUTRAL_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 2) - (BASE_SIZE / 2),
			(WINDOW_HEIGHT / 2) + (BASE_SIZE / 2)},
		BASE_SIZE);

	base_init(
		&bases[3],
		3,
		NEUTRAL_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 3 * 2) - (BASE_SIZE / 2),
			(WINDOW_HEIGHT / 2) + (BASE_SIZE / 2)},
		BASE_SIZE);

	Vertex base_vertices[BASE_COUNT * 6] = {0};
	generate_quad(base_vertices, bases[0].pos, bases[0].size, BLUE_FACTION);
	generate_quad(base_vertices + 6, bases[4].pos, bases[4].size, RED_FACTION);
	generate_quad(base_vertices + 12, bases[1].pos, bases[1].size, NEUTRAL_FACTION);
	generate_quad(base_vertices + 18, bases[2].pos, bases[2].size, NEUTRAL_FACTION);
	generate_quad(base_vertices + 24, bases[3].pos, bases[3].size, NEUTRAL_FACTION);

	engine_init(WINDOW_WIDTH, WINDOW_HEIGHT);

	renderer_shader_new(SHAPE_PIPELINE_IDX, "./triangle.metal", VERTEX_SHADER, 1);
	renderer_shader_new(SHAPE_PIPELINE_IDX, "./triangle.metal", FRAGMENT_SHADER, 0);
	renderer_buffer_new(
		SHAPE_BUFF_IDX,
		base_vertices,
		sizeof(Vertex),
		sizeof(base_vertices) / sizeof(Vertex));
	renderer_pipeline_new(
		SHAPE_PIPELINE_IDX,
		sizeof(Vertex),
		sizeof(Vec2),
		TRIANGLE_PRIMITIVE);

	Vertex line_vertices[] = {
		(Vertex){
			.pos = (Vec2){0, 0},
			.color = (Color){0, 255, 0, 255},
		},
		(Vertex){
			.pos = (Vec2){0, 0},
			.color = (Color){0, 255, 0, 255},
		},
	};
	renderer_shader_new(LINE_PIPELINE_IDX, "./line.metal", VERTEX_SHADER, 2);
	renderer_shader_new(LINE_PIPELINE_IDX, "./line.metal", FRAGMENT_SHADER, 0);
	renderer_buffer_new(
		LINE_BUFF_IDX,
		line_vertices,
		sizeof(Vertex),
		sizeof(line_vertices) / sizeof(Vertex));
	renderer_pipeline_new(
		LINE_PIPELINE_IDX,
		sizeof(Vertex),
		sizeof(Vec2),
		LINE_PRIMITIVE);

	Vertex edge_vertices[16];
	generate_edge_vertices(edge_vertices, bases, nodes, edges);
	renderer_shader_new(EDGE_PIPELINE_IDX, "./edge.metal", VERTEX_SHADER, 1);
	renderer_shader_new(EDGE_PIPELINE_IDX, "./edge.metal", FRAGMENT_SHADER, 0);
	renderer_buffer_new(
		EDGE_BUFF_IDX,
		edge_vertices,
		sizeof(Vertex),
		sizeof(edge_vertices) / sizeof(Vertex));
	renderer_pipeline_new(
		EDGE_PIPELINE_IDX,
		sizeof(Vertex),
		sizeof(Vec2),
		LINE_PRIMITIVE);

	Vertex unit_vertices[3];
	generate_triangle(unit_vertices, (Vec2){100, 100}, 15);
	renderer_shader_new(UNIT_PIPELINE_IDX, "./unit.metal", VERTEX_SHADER, 2);
	renderer_shader_new(UNIT_PIPELINE_IDX, "./unit.metal", FRAGMENT_SHADER, 0);
	renderer_buffer_new(
		UNIT_BUFF_IDX,
		unit_vertices,
		sizeof(Vertex),
		sizeof(unit_vertices) / sizeof(Vertex));
	renderer_pipeline_new(
		UNIT_PIPELINE_IDX,
		sizeof(Vertex),
		sizeof(Vec2),
		TRIANGLE_PRIMITIVE);

	Game game = game_new(WINDOW_WIDTH, WINDOW_HEIGHT, bases, nodes, edges);

	App app = {
		.state = &game,
		.update = game_update,
		.render = game_render};

	engine_run(&app);

	game_release(&game);
	engine_release();

	return 0;
}