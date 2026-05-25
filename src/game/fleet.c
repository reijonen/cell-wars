#include <SDL3/SDL.h>

#include "fleet.h"
#include "game.h"

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

static EID find_shared_edge(Game *game, Base *first, Base *second)
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

			if (first->edge == second->edge && first->dir != second->dir && first->faction != second->faction)
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
