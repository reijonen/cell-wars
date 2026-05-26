#include <SDL3/SDL.h>

#include "game.h"
#include "base.h"

// TODO: tmp
#include "engine/renderer.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define PROJECTION_UNIFORM_IDX 0
#define LINE_UNIFORM_IDX 1
#define BASE_FACTIONS_UNIFORM_IDX 2
#define HEALTHS_UNIFORM_IDX 2
#define HEALTH_FACTIONS_UNIFORM_IDX 3
#define FLEET_SIZE_UNIFORM_IDX 2

#define SHAPE_PIPELINE_IDX 0
#define SHAPE_BUFF_IDX 0
#define LINE_PIPELINE_IDX 1
#define LINE_BUFF_IDX 1
#define EDGE_PIPELINE_IDX 2
#define EDGE_BUFF_IDX 2
#define UNIT_PIPELINE_IDX 3
#define UNIT_BUFF_IDX 3
#define HEALTH_PIPELINE_IDX 4
#define HEALTH_BUFF_IDX 4

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

static void process_input(Game *game, SDL_Event *events, unsigned int event_count)
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

Game game_new(unsigned window_width, unsigned window_height, Base *bases, Node *nodes, Edge *edges)
{
	Game game;
	game.camera = camera_init(window_width, window_height);
	game.bases = bases;
	game.nodes = nodes;
	game.edges = edges;
	game.is_dragging = false;
	game.line = (Vec4){0};
	game.fleets_active = 0;

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
	process_input(game, events, event_count);

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
	renderer_draw(EDGE_PIPELINE_IDX, EDGE_BUFF_IDX, 16, 1);
	renderer_update_uniform(BASE_FACTIONS_UNIFORM_IDX, factions, sizeof(factions));
	renderer_draw(SHAPE_PIPELINE_IDX, SHAPE_BUFF_IDX, 30, 1);

	renderer_update_uniform(HEALTHS_UNIFORM_IDX, healths, sizeof(healths));
	renderer_update_uniform(HEALTH_FACTIONS_UNIFORM_IDX, factions, sizeof(factions));
	renderer_draw(HEALTH_PIPELINE_IDX, HEALTH_BUFF_IDX, BASE_COUNT, max_health);

	for (unsigned i = 0; i < game->fleets_active; i++)
	{
		renderer_update_uniform(LINE_UNIFORM_IDX, &game->fleets[i].shape, sizeof(Vec4));
		uint32_t fleet_size = (uint32_t)game->fleets[i].size;
		renderer_update_uniform(FLEET_SIZE_UNIFORM_IDX, &fleet_size, sizeof(uint32_t));
		renderer_draw(UNIT_PIPELINE_IDX, UNIT_BUFF_IDX, 3, fleet_size);
	}

	if (game->is_dragging)
	{
		renderer_update_uniform(LINE_UNIFORM_IDX, &game->line, sizeof(Vec4));
		renderer_draw(LINE_PIPELINE_IDX, LINE_BUFF_IDX, 2, 1);
	}
}

void game_release(Game *game)
{
}
