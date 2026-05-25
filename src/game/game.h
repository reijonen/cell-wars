#pragma once

#include "../engine/input.h"
#include "base.h"
#include "../engine/camera.h"
#include "engine/types/vec4.h"

#define BASE_COUNT 5

typedef enum AttackDirection
{
	LEFT,
	RIGHT
} AttackDirection;

typedef struct AttackFleet
{
	EID edge;
	AttackDirection dir;
	unsigned size;
	Vec4 shape;
	Faction faction;
} AttackFleet;

typedef struct Game
{
	Base *bases;
	AttackFleet fleets[128];
	unsigned char fleets_active;
	Node *nodes;
	Edge *edges;
	Camera camera;
	Vec4 line;
	bool is_dragging;
} Game;

Game game_new(unsigned window_width, unsigned window_height, Base *bases, Node *nodes, Edge *edges);
void game_update(void *state, SDL_Event *events, unsigned int event_count, double delta_time);
void game_render(void *state);
void game_release(Game *game);