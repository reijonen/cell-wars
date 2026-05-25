#include "base.h"

#include <stdio.h>

void base_init(Base *b, NID id, Faction faction, Vec2 pos, unsigned size)
{
	b->faction = faction;
	b->health = 10;
	b->pos = pos;
	b->size = size;
	b->nid = id;
}

void base_update(Base *base)
{
	// base->health += 1;
}

bool base_hit_test(Base *base, Vec2 hit)
{
	// printf("Hit test x: %f, y: %f\n", hit.x, hit.y);
	if ((hit.x >= base->pos.x && hit.x <= (base->pos.x + base->size)) && (hit.y <= base->pos.y && hit.y >= (base->pos.y - base->size)))
		return true;

	return false;
}

void base_take_damage(Base *base, Faction from, size_t amount)
{
	if (amount == 0)
	{
		return;
	}

	if (base->faction == from)
	{
		base->health += amount;
		return;
	}

	if (base->health == 0)
	{
		base->faction = from;
		base->health = amount;
		return;
	}

	if (amount >= base->health)
	{
		size_t overflow = amount - base->health;
		base->faction = from;
		base->health = overflow;
		return;
	}

	base->health -= amount;
}
