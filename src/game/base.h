#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "faction.h"
#include "../engine/types/vec2.h"

typedef struct Base
{
	Faction faction;
	size_t health;
	double regen_accum_seconds;
	Vec2 pos;
	unsigned size;
	// const unsigned char radius;
} Base;

void base_init(Base *b, Faction faction, Vec2 pos, unsigned size);

void base_update(Base *base, double delta_time);

bool base_hit_test(Base *base, Vec2 hit);

// TODO: handle color conversion in shader
void base_take_damage(Base *base, Faction from, size_t amount);
void base_render(uint32_t *factions, unsigned base_count);
void base_health_render(uint32_t *healths, uint32_t *factions, unsigned base_count, unsigned max_health);
