#pragma once

#include <stddef.h>

#include "faction.h"
#include "../engine/types/vec2.h"

typedef struct Base
{
	Faction faction;
	size_t health;
	Vec2 pos;
	const unsigned char radius;
} Base;

Base base_new();

void base_update(Base *base);

void base_take_damage(Base *base, Faction from);