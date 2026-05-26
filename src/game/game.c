#include <SDL3/SDL.h>

#include "game.h"
#include "base.h"
#include "graph.h"
#include "fleet.h"
#include "game_render_ids.h"

// TODO: tmp
#include "engine/renderer.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

Base *first_base = NULL;
static const float WINDOW_HEIGHT = 720.0f;

static Vec2 mouse_event_hit(const SDL_MouseButtonEvent *event)
{
	return (Vec2){event->x, WINDOW_HEIGHT - event->y};
}

static Base *find_base_at_hit(Game *game, Vec2 hit)
{
	for (unsigned i = 0; i < BASE_COUNT; i++)
	{
		if (base_hit_test(&(game->bases[i]), hit))
		{
			return &(game->bases[i]);
		}
	}

	return NULL;
}

static void handle_mouse_button_down(Game *game, const SDL_MouseButtonEvent *event)
{
	if (event->button != 1)
	{
		return;
	}

	Vec2 hit = mouse_event_hit(event);
	Base *hit_base = find_base_at_hit(game, hit);
	if (hit_base == NULL)
	{
		return;
	}

	game->is_dragging = true;
	game->line.x = hit.x;
	game->line.y = hit.y;
	game->line.z = hit.x;
	game->line.w = hit.y;
	first_base = hit_base;
}

static void handle_mouse_button_up(Game *game, const SDL_MouseButtonEvent *event)
{
	if (event->button != 1)
	{
		return;
	}

	Vec2 hit = mouse_event_hit(event);
	Base *second_base = find_base_at_hit(game, hit);
	if (second_base != NULL)
	{
		if (first_base != NULL && first_base != second_base && first_base->faction != NEUTRAL_FACTION)
		{
			fleets_new(game, first_base, second_base);
		}
	}

	game->is_dragging = false;
}

static void handle_mouse_motion(Game *game, const SDL_MouseMotionEvent *event)
{
	if (!game->is_dragging)
	{
		return;
	}

	game->line.z = event->x;
	game->line.w = WINDOW_HEIGHT - event->y;
}

static void game_process_input(Game *game, SDL_Event *events, unsigned int event_count)
{
	for (unsigned int i = 0; i < event_count; i++)
	{
		switch (events[i].type)
		{
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			handle_mouse_button_down(game, &events[i].button);
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			handle_mouse_button_up(game, &events[i].button);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			handle_mouse_motion(game, &events[i].motion);
			break;
		default:
			break;
		}
	}
}

static void line_render(Game *game)
{
	if (!game->is_dragging)
	{
		return;
	}

	renderer_update_uniform(LINE_UNIFORM_IDX, &game->line, sizeof(Vec4));
	renderer_draw(LINE_PIPELINE_IDX, LINE_BUFF_IDX, 2, 1);
}

Game game_new(unsigned window_width, unsigned window_height, unsigned base_size)
{
	Game game;
	game.camera = camera_init(window_width, window_height);
	game.is_dragging = false;
	game.line = (Vec4){0};
	game.fleets_active = 0;
	graph_init_nodes(game.nodes, BASE_COUNT);
	graph_init_edges(game.edges, EDGE_COUNT);
	graph_build_default(game.nodes, game.edges);
	bases_init_default(game.bases, window_width, window_height, base_size);

	for (unsigned i = 0; i < BASE_COUNT; i++)
	{
		assert(game.nodes[i].id < BASE_COUNT);
		for (unsigned j = 0; j < game.nodes[i].link_count; j++)
		{
			assert(game.nodes[i].links[j].to < BASE_COUNT);
		}
	}

	return game;
}

void game_update(void *state, SDL_Event *events, unsigned int event_count, double delta_time)
{
	Game *game = (Game *)state;
	game_process_input(game, events, event_count);

	for (unsigned i = 0; i < BASE_COUNT; i++)
	{
		base_update(&game->bases[i], delta_time);
	}

	unsigned i = 0;
	while (i < game->fleets_active)
	{
		if (!fleet_update(game, i))
		{
			i++;
		}
	}
}

void game_render(void *state)
{
	Game *game = (Game *)state;
	uint32_t healths[BASE_COUNT] = {0};
	uint32_t factions[BASE_COUNT] = {0};
	unsigned max_health = 0;
	for (unsigned i = 0; i < BASE_COUNT; i++)
	{
		healths[i] = (uint32_t)game->bases[i].health;
		factions[i] = (uint32_t)game->bases[i].faction;
		if (healths[i] > max_health)
		{
			max_health = healths[i];
		}
	}

	renderer_update_uniform(PROJECTION_UNIFORM_IDX, &game->camera.projection, sizeof(mat4x4));
	edges_render();
	base_render(factions, BASE_COUNT);
	base_health_render(healths, factions, BASE_COUNT, max_health);
	fleet_render(game->fleets, game->fleets_active);
	line_render(game);
}

void game_release(Game *game)
{
}
