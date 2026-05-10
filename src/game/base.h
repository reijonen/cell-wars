#pragma once

#include <stddef.h>

#include "faction.h"
#include "../engine/types/vec2.h"

struct Base
{
	enum Faction faction;
	size_t health;
	struct Vec2 pos;
	const unsigned char radius;
};

struct Base base_new();

void base_update(struct Base *base);

void base_take_damage(struct Base *base, enum Faction from);