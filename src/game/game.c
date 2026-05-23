#include <SDL3/SDL.h>

#include "game.h"
#include "base.h"

// TODO: tmp
#include "engine/renderer.h"
#include <stdlib.h>

#define PROJECTION_UNIFORM_IDX 0
#define LINE_UNIFORM_IDX 1

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
	af.size = 1; // TODO
	af.shape = (Vec4){
		top_center_x,
		top_center_y,
		direction.x,
		direction.y,
	};

	game->fleets[game->fleets_active++] = af;
}

void fleets_update(Game *game)
{
	for (unsigned i = 0; i < game->fleets_active; i++)
	{
		AttackFleet *fleet = &(game->fleets[i]);

		fleet->shape.x += fleet->shape.z * 0.25;
		fleet->shape.y += fleet->shape.w * 0.25;

		Base *begin = NULL;
		Base *end = NULL;
		for (unsigned j = 0; j < BASE_COUNT; j++)
		{
			if (game->bases[j].nid == game->edges[fleet->edge].endpoints[0])
			{
				if (fleet->dir == RIGHT)
				{
					end = &(game->bases[j]);
				}
				else
				{
					begin = &(game->bases[j]);
				}
			}

			if (game->bases[j].nid == game->edges[fleet->edge].endpoints[1])
			{
				if (fleet->dir == LEFT)
				{
					end = &(game->bases[j]);
				}
				else
				{
					begin = &(game->bases[j]);
				}
			}
		}

		float dx = end->pos.x - begin->pos.x;
		float dy = end->pos.y - begin->pos.y;

		printf("shape x: %f\n", fleet->shape.x);
		printf("end x: %f\n", end->pos.x);

		if (dx > 0)
		{
			if (fleet->shape.x > (end->pos.x + end->size / 2))
			{
				game->fleets_active = 0;
			}
		}
		else if (dx < 0)
		{
			if (fleet->shape.x < (end->pos.x + end->size / 2))
			{
				game->fleets_active = 0;
			}
		}

		if (dy > 0)
		{
			if (fleet->shape.y > (end->pos.y - end->size / 2))
			{
				game->fleets_active = 0;
			}
		}
		else if (dy < 0)
		{
			if (fleet->shape.y < (end->pos.y - end->size / 2))
			{
				game->fleets_active = 0;
			}
		}
	}
}

void game_update(void *state, SDL_Event *events, unsigned int event_count, double delta_time)
{
	Game *game = (Game *)state;
	// printf("Line x: %f, y: %f, z: %f, w: %f\n", game->line.x, game->line.y, game->line.z, game->line.w);

	for (unsigned int i = 0; i < event_count; i++)
	{
		switch (events[i].type)
		{
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			if (events[i].button.button == 1)
			{
				bool hitted = false;
				// TODO: WINDOW_HEIGHT
				Vec2 hit = {events[i].button.x, 720 - events[i].button.y};
				for (unsigned i = 0; i < BASE_COUNT; i++)
				{
					if (base_hit_test(&(game->bases[i]), hit))
					{
						printf("HIT xy: %f, %f\n", hit.x, hit.y);
						hitted = true;
						game->is_dragging = true;
						game->line.x = hit.x;
						game->line.y = hit.y;
						game->line.z = hit.x;
						game->line.w = hit.y;
						first_base = &(game->bases[i]);
					}
				}
				if (!hitted)
				{
					game->line = (Vec4){0};
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
						fleets_new(game, first_base, second_base);
					}
				}
				game->is_dragging = false;
				game->line = (Vec4){0};
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
		fleets_update(game);
	}
}

void game_render(void *state)
{
	Game *game = (Game *)state;

	renderer_update_uniform(PROJECTION_UNIFORM_IDX, &game->camera.projection, sizeof(mat4x4));
	renderer_draw(2, 2, 16, 0);
	renderer_draw(0, 0, 32, 0);

	renderer_update_uniform(1, &game->fleets[0].shape, sizeof(Vec4));
	renderer_draw(3, 3, 3, 3);

	renderer_update_uniform(LINE_UNIFORM_IDX, &game->line, sizeof(Vec4));
	renderer_draw(1, 1, 2, 1);
}

void game_release(Game *game)
{
}