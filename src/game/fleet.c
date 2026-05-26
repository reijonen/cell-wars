#include <SDL3/SDL.h>
#include <assert.h>
#include <stdint.h>

#include "fleet.h"
#include "game.h"
#include "game_render_ids.h"
#include "engine/renderer.h"

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
	NID first = game->edges[fleet->edge].endpoints[0];
	NID second = game->edges[fleet->edge].endpoints[1];
	if (first >= BASE_COUNT || second >= BASE_COUNT)
	{
		*begin = NULL;
		*end = NULL;
		return false;
	}
	Base *first_base = &game->bases[first];
	Base *second_base = &game->bases[second];

	if (fleet->dir == RIGHT)
	{
		*begin = second_base;
		*end = first_base;
	}
	else
	{
		*begin = first_base;
		*end = second_base;
	}

	return true;
}

static void fleet_step_motion(AttackFleet *fleet)
{
	fleet->shape.x += fleet->shape.z;
	fleet->shape.y += fleet->shape.w;
}

static bool fleet_reached_end(const AttackFleet *fleet, const Base *begin, const Base *end)
{
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

	return end_reached;
}

static bool fleet_resolve_arrival(Game *game, unsigned index)
{
	AttackFleet *fleet = &(game->fleets[index]);
	Base *begin = NULL;
	Base *end = NULL;
	if (!fleet_find_endpoints(game, fleet, &begin, &end))
	{
		fleet_remove(game, index);
		return true;
	}

	if (!fleet_reached_end(fleet, begin, end))
	{
		return false;
	}

	base_take_damage(end, fleet->faction, fleet->size);
	fleet_remove(game, index);
	return true;
}

static bool fleet_resolve_clashes(Game *game, unsigned index)
{
	AttackFleet *fleet = &(game->fleets[index]);
	unsigned j = index + 1;
	while (j < game->fleets_active)
	{
		AttackFleet *second = &(game->fleets[j]);
		if (fleet->edge == second->edge && fleet->dir != second->dir && fleet->faction != second->faction)
		{
			float cdx = fleet->shape.x - second->shape.x;
			float cdy = fleet->shape.y - second->shape.y;
			float distance_sq = (cdx * cdx) + (cdy * cdy);
			if (distance_sq <= (FLEET_CLASH_DISTANCE * FLEET_CLASH_DISTANCE))
			{
				if (fleet->size == second->size)
				{
					fleet_remove(game, j);
					fleet_remove(game, index);
					return true;
				}
				else if (fleet->size > second->size)
				{
					fleet->size -= second->size;
					fleet_remove(game, j);
					continue;
				}
				else
				{
					second->size -= fleet->size;
					fleet_remove(game, index);
					return true;
				}
			}
		}
		j++;
	}

	return false;
}

void fleets_new(Game *game, Base *first, Base *second)
{
	if (game->fleets_active >= FLEET_MAX_ACTIVE)
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

	size_t first_idx = (size_t)(first - game->bases);
	size_t second_idx = (size_t)(second - game->bases);
	assert(first_idx < BASE_COUNT);
	assert(second_idx < BASE_COUNT);

	Node *first_node = &(game->nodes[first_idx]);
	EID shared_edge = graph_find_edge_between(first_node, (NID)second_idx);
	if (shared_edge == INVALID_EID)
		return;

	AttackFleet af = {0};
	af.edge = shared_edge;
	af.dir = game->edges[shared_edge].endpoints[0] == first_idx ? LEFT : RIGHT;
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

bool fleet_update(Game *game, unsigned index)
{
	if (index >= game->fleets_active)
		return false;

	fleet_step_motion(&(game->fleets[index]));
	if (fleet_resolve_arrival(game, index))
		return true;
	if (fleet_resolve_clashes(game, index))
		return true;

	return false;
}

void fleet_render(AttackFleet *fleets, unsigned fleets_active)
{
	for (unsigned i = 0; i < fleets_active; i++)
	{
		renderer_update_uniform(LINE_UNIFORM_IDX, &fleets[i].shape, sizeof(Vec4));
		uint32_t fleet_size = (uint32_t)fleets[i].size;
		renderer_update_uniform(FLEET_SIZE_UNIFORM_IDX, &fleet_size, sizeof(uint32_t));
		renderer_draw(UNIT_PIPELINE_IDX, UNIT_BUFF_IDX, 3, fleet_size);
	}
}

void fleet_build_vertices(Vertex *vertices, Vec2 top_center, unsigned size)
{
	Color c = {255, 255, 0, 255};

	vertices[0].pos.x = top_center.x;
	vertices[0].pos.y = top_center.y;
	vertices[0].color = c;

	vertices[1].pos.x = top_center.x + size / 2;
	vertices[1].pos.y = top_center.y - size;
	vertices[1].color = c;

	vertices[2].pos.x = top_center.x - size / 2;
	vertices[2].pos.y = top_center.y - size;
	vertices[2].color = c;
}
