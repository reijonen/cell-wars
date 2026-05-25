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
#define FLEET_CLASH_DISTANCE 4.0f

Base *first_base = NULL;

static void fleet_remove(Game *game, unsigned index)
{
	if (game->fleets_active == 0)
		return;

	unsigned last = game->fleets_active - 1;
	if (index != last)
	{
		game->fleets[index] = game->fleets[last];
	}
	game->fleets_active--;
}

static bool fleet_find_endpoints(Game *game, AttackFleet *fleet, Base **begin, Base **end)
{
	*begin = NULL;
	*end = NULL;

	for (unsigned j = 0; j < BASE_COUNT; j++)
	{
		if (game->bases[j].nid == game->edges[fleet->edge].endpoints[0])
		{
			if (fleet->dir == RIGHT)
			{
				*end = &(game->bases[j]);
			}
			else
			{
				*begin = &(game->bases[j]);
			}
		}

		if (game->bases[j].nid == game->edges[fleet->edge].endpoints[1])
		{
			if (fleet->dir == LEFT)
			{
				*end = &(game->bases[j]);
			}
			else
			{
				*begin = &(game->bases[j]);
			}
		}
	}

	return *begin != NULL && *end != NULL;
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

	return game;
}

EID find_shared_edge(Game *game, Base *first, Base *second)
{
	Node *first_node = &(game->nodes[first->nid]);
	Node *second_node = &(game->nodes[second->nid]);

	for (unsigned i = 0; i < first_node->edge_count; i++)
	{
		EID feid = first_node->edges[i];

		for (unsigned j = 0; j < second_node->edge_count; j++)
		{
			EID seid = second_node->edges[j];

			if (feid == seid)
				return feid;
		}
	}

	// TODO: fix
	return 9999;
}

void fleets_new(Game *game, Base *first, Base *second)
{
	if (game->fleets_active >= 128)
		return;

	if (first->health <= 1)
		return;
	size_t sent_amount = (first->health + 1) / 2;

	float top_center_x = first->pos.x + first->size / 2;
	float top_center_y = first->pos.y - first->size / 2;

	float dx = second->pos.x - first->pos.x;
	float dy = second->pos.y - first->pos.y;
	float length = SDL_sqrt(dx * dx + dy * dy);
	Vec2 direction = (Vec2){dx / length, dy / length};

	EID shared_edge = find_shared_edge(game, first, second);
	if (shared_edge == 9999)
		return;

	AttackFleet af = {0};
	af.edge = shared_edge;
	af.dir = game->edges[shared_edge].endpoints[0] == first->nid ? LEFT : RIGHT;
	af.size = sent_amount;
	af.shape = (Vec4){
		top_center_x,
		top_center_y,
		direction.x,
		direction.y,
	};
	af.faction = first->faction;

	first->health -= sent_amount;

	game->fleets[game->fleets_active++] = af;
}

void fleets_update(Game *game)
{
	unsigned i = 0;
	while (i < game->fleets_active)
	{
		AttackFleet *fleet = &(game->fleets[i]);

		fleet->shape.x += fleet->shape.z;
		fleet->shape.y += fleet->shape.w;

		Base *begin = NULL;
		Base *end = NULL;
		if (!fleet_find_endpoints(game, fleet, &begin, &end))
		{
			fleet_remove(game, i);
			continue;
		}

		float dx = end->pos.x - begin->pos.x;
		float dy = end->pos.y - begin->pos.y;

		bool end_reached = false;
		if (dx > 0)
		{
			if (fleet->shape.x > (end->pos.x + end->size / 2))
			{
				end_reached = true;
			}
		}
		else if (dx < 0)
		{
			if (fleet->shape.x < (end->pos.x + end->size / 2))
			{
				end_reached = true;
			}
		}

		if (dy > 0)
		{
			if (fleet->shape.y > (end->pos.y - end->size / 2))
			{
				end_reached = true;
			}
		}
		else if (dy < 0)
		{
			if (fleet->shape.y < (end->pos.y - end->size / 2))
			{
				end_reached = true;
			}
		}

		if (end_reached)
		{
			base_take_damage(end, fleet->faction, fleet->size);
			fleet_remove(game, i);
			continue;
		}

		i++;
	}

	i = 0;
	while (i < game->fleets_active)
	{
		AttackFleet *first = &(game->fleets[i]);
		bool first_removed = false;

		unsigned j = i + 1;
		while (j < game->fleets_active)
		{
			AttackFleet *second = &(game->fleets[j]);

			if (first->edge == second->edge && first->dir != second->dir)
			{
				float dx = first->shape.x - second->shape.x;
				float dy = first->shape.y - second->shape.y;
				float distance_sq = (dx * dx) + (dy * dy);
				if (distance_sq <= (FLEET_CLASH_DISTANCE * FLEET_CLASH_DISTANCE))
				{
					if (first->size == second->size)
					{
						fleet_remove(game, j);
						fleet_remove(game, i);
						first_removed = true;
						break;
					}
					else if (first->size > second->size)
					{
						first->size -= second->size;
						fleet_remove(game, j);
						continue;
					}
					else
					{
						second->size -= first->size;
						fleet_remove(game, i);
						first_removed = true;
						break;
					}
				}
			}

			j++;
		}

		if (!first_removed)
		{
			i++;
		}
	}
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
		renderer_draw(UNIT_PIPELINE_IDX, UNIT_BUFF_IDX, 3, 1);
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
