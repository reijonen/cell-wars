#include "engine/engine.h"
#include "game/game.h"
#include "game/game_render_ids.h"
#include "engine/camera.h"

#include "game/graph.h"
#include <string.h>

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

void generate_edge_vertices(Vertex *v, Base *bases, Node *nodes, Edge *edges)
{
	(void)nodes;

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

		Base *left_base = &bases[left];
		Base *right_base = &bases[right];

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

#define BASE_SIZE 50
#define EDGE_VERTEX_COUNT (EDGE_COUNT * 2)

static void init_nodes(Node *nodes, size_t count)
{
	memset(nodes, 0, sizeof(Node) * count);
	for (size_t i = 0; i < count; i++)
	{
		nodes[i].id = (NID)i;
	}
}

static void init_edges(Edge *edges, size_t count)
{
	memset(edges, 0, sizeof(Edge) * count);
	for (size_t i = 0; i < count; i++)
	{
		edges[i].id = (EID)i;
	}
}

static void build_default_graph(Node *nodes, Edge *edges)
{
	graph_connect_undirected(nodes, edges, 0, 0, 1);
	graph_connect_undirected(nodes, edges, 1, 0, 2);
	graph_connect_undirected(nodes, edges, 2, 0, 3);
	graph_connect_undirected(nodes, edges, 3, 1, 2);
	graph_connect_undirected(nodes, edges, 4, 2, 3);
	graph_connect_undirected(nodes, edges, 5, 1, 4);
	graph_connect_undirected(nodes, edges, 6, 2, 4);
	graph_connect_undirected(nodes, edges, 7, 3, 4);
}

static void init_bases(Base *bases)
{
	// Identity invariant: base index equals node id.
	base_init(
		&bases[0],
		BLUE_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 2) - (BASE_SIZE / 2),
			200 + (BASE_SIZE / 2)},
		BASE_SIZE);

	base_init(
		&bases[4],
		RED_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 2) - (BASE_SIZE / 2),
			(WINDOW_HEIGHT - 200) + (BASE_SIZE / 2)},
		BASE_SIZE);

	base_init(
		&bases[1],
		NEUTRAL_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 3) - (BASE_SIZE / 2),
			(WINDOW_HEIGHT / 2) + (BASE_SIZE / 2)},
		BASE_SIZE);

	base_init(
		&bases[2],
		NEUTRAL_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 2) - (BASE_SIZE / 2),
			(WINDOW_HEIGHT / 2) + (BASE_SIZE / 2)},
		BASE_SIZE);

	base_init(
		&bases[3],
		NEUTRAL_FACTION,
		(Vec2){
			(WINDOW_WIDTH / 3 * 2) - (BASE_SIZE / 2),
			(WINDOW_HEIGHT / 2) + (BASE_SIZE / 2)},
		BASE_SIZE);
}

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
	Edge edges[EDGE_COUNT];
	Base bases[BASE_COUNT];
	init_nodes(nodes, BASE_COUNT);
	init_edges(edges, EDGE_COUNT);
	build_default_graph(nodes, edges);
	init_bases(bases);

	Vertex base_vertices[BASE_COUNT * 6] = {0};
	for (unsigned i = 0; i < BASE_COUNT; i++)
	{
		generate_quad(base_vertices + (i * 6), bases[i].pos, bases[i].size, bases[i].faction);
	}

	engine_init(WINDOW_WIDTH, WINDOW_HEIGHT);

	renderer_shader_new(SHAPE_PIPELINE_IDX, "./base.metal", VERTEX_SHADER, 3);
	renderer_shader_new(SHAPE_PIPELINE_IDX, "./base.metal", FRAGMENT_SHADER, 0);
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

	Vertex edge_vertices[EDGE_VERTEX_COUNT];
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
	renderer_shader_new(UNIT_PIPELINE_IDX, "./unit.metal", VERTEX_SHADER, 3);
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

	Vertex health_vertices[BASE_COUNT] = {0};
	for (unsigned i = 0; i < BASE_COUNT; i++)
	{
		health_vertices[i].pos = (Vec2){
			bases[i].pos.x + bases[i].size / 2,
			bases[i].pos.y - bases[i].size / 2};
		health_vertices[i].color = (Color){255, 255, 255, 255};
	}

	renderer_shader_new(HEALTH_PIPELINE_IDX, "./health.metal", VERTEX_SHADER, 4);
	renderer_shader_new(HEALTH_PIPELINE_IDX, "./health.metal", FRAGMENT_SHADER, 0);
	renderer_buffer_new(
		HEALTH_BUFF_IDX,
		health_vertices,
		sizeof(Vertex),
		sizeof(health_vertices) / sizeof(Vertex));
	renderer_pipeline_new(
		HEALTH_PIPELINE_IDX,
		sizeof(Vertex),
		sizeof(Vec2),
		POINT_PRIMITIVE);

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
