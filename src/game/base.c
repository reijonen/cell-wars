#include "base.h"

#include <stdio.h>

void base_init(Base *b, NID id, Faction faction, Vec2 pos, unsigned size)
{
	b->faction = faction;
	b->health = 100;
	b->pos = pos;
	b->size = size;
	b->nid = id;
}

void base_update(Base *base)
{
	base->health += 1;
}

bool base_hit_test(Base *base, Vec2 hit)
{
	// printf("Hit test x: %f, y: %f\n", hit.x, hit.y);
	if ((hit.x >= base->pos.x && hit.x <= (base->pos.x + base->size)) && (hit.y <= base->pos.y && hit.y >= (base->pos.y - base->size)))
		return true;

	return false;
}

void base_take_damage(Base *base, Faction from)
{
	if (base->health == 0)
	{
		base->health += 1;
		base->faction = from;
	}
	else
	{
		base->health -= 1;
		if (base->health == 0)
		{
			base->faction = NEUTRAL_FACTION;
		}
	}
}
