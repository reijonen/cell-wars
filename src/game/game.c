#include <SDL3/SDL.h>

#include "game.h"
#include "base.h"

// TODO: tmp
#include "engine/renderer.h"
#include <stdlib.h>
#include <stdint.h>

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

	return game;
}

void game_update(void *state, SDL_Event *events, unsigned int event_count, double delta_time)
{
	Game *game = (Game *)state;
	static double regen_accum_seconds = 0.0;
	regen_accum_seconds += delta_time;
	// printf("Line x: %f, y: %f, z: %f, w: %f\n", game->line.x, game->line.y, game->line.z, game->line.w);

	for (unsigned int i = 0; i < event_count; i++)
	{
		switch (events[i].type)
		{
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			if (events[i].button.button == 1)
			{
				// TODO: WINDOW_HEIGHT
				Vec2 hit = {events[i].button.x, 720 - events[i].button.y};
				for (unsigned i = 0; i < BASE_COUNT; i++)
				{
					if (base_hit_test(&(game->bases[i]), hit))
					{
						printf("HIT xy: %f, %f\n", hit.x, hit.y);
						game->is_dragging = true;
						game->line.x = hit.x;
						game->line.y = hit.y;
						game->line.z = hit.x;
						game->line.w = hit.y;
						first_base = &(game->bases[i]);
					}
				}
			}

			continue;
		}

		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			if (events[i].button.button == 1)
			{
				// TODO: WINDOW_HEIGHT
				Vec2 hit = {events[i].button.x, 720 - events[i].button.y};
				for (unsigned i = 0; i < BASE_COUNT; i++)
				{
					if (base_hit_test(&(game->bases[i]), hit))
					{
						printf("HIT xy: %f, %f\n", hit.x, hit.y);
						Base *second_base = &(game->bases[i]);
						if (first_base != NULL && first_base != second_base && first_base->faction != NEUTRAL_FACTION)
						{
							fleets_new(game, first_base, second_base);
						}
					}
				}
				game->is_dragging = false;
			}

			continue;
		}

		case SDL_EVENT_MOUSE_MOTION:
		{
			if (game->is_dragging)
			{
				game->line.z = events[i].button.x;
				game->line.w = 720 - events[i].button.y;
			}
		}
		}
	}

	for (unsigned i = 0; i < BASE_COUNT; i++)
	{
		base_update(&game->bases[i]);
	}
	fleets_update(game);

	// TODO: move to base_update
	while (regen_accum_seconds >= 1.0)
	{
		for (unsigned i = 0; i < BASE_COUNT; i++)
		{
			if (game->bases[i].faction != NEUTRAL_FACTION)
			{
				game->bases[i].health += 1;
			}
		}
		regen_accum_seconds -= 1.0;
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
