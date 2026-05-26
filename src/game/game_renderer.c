#include "game_renderer.h"

#include "game_render_ids.h"
#include "render_types.h"
#include "base.h"
#include "fleet.h"
#include "graph.h"
#include "engine/renderer.h"

static void init_base_pass(const Game *game)
{
	Vertex base_vertices[BASE_COUNT * 6] = {0};
	base_build_vertices(base_vertices, game->bases, BASE_COUNT);

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
}

static void init_line_pass(void)
{
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
}

static void init_edge_pass(const Game *game)
{
	Vertex edge_vertices[EDGE_COUNT * 2] = {0};
	graph_build_edge_vertices(edge_vertices, game->bases, game->edges, EDGE_COUNT);

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
}

static void init_fleet_pass(void)
{
	Vertex unit_vertices[3] = {0};
	fleet_build_vertices(unit_vertices, (Vec2){100, 100}, 15);

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
}

static void init_health_pass(const Game *game)
{
	Vertex health_vertices[BASE_COUNT] = {0};
	base_health_build_vertices(health_vertices, game->bases, BASE_COUNT);

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
}

void game_renderer_init(GameRenderer *renderer, Game *game)
{
	init_base_pass(game);
	init_line_pass();
	init_edge_pass(game);
	init_fleet_pass();
	init_health_pass(game);
	renderer->initialized = true;
}

void game_renderer_release(GameRenderer *renderer)
{
	renderer->initialized = false;
}
