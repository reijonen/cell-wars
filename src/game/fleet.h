#pragma once

#include <stddef.h>

#include "graph.h"
#include "base.h"
#include "engine/types/vec4.h"

#define FLEET_MAX_ACTIVE 128
#define FLEET_CLASH_DISTANCE 4.0f

typedef enum AttackDirection
{
	LEFT,
	RIGHT
} AttackDirection;

typedef struct AttackFleet
{
	EID edge;
	AttackDirection dir;
	size_t size;
	Vec4 shape;
	Faction faction;
} AttackFleet;

typedef struct Game Game;

void fleets_new(Game *game, Base *first, Base *second);
void fleets_update(Game *game);
