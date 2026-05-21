#include "base.h"

Base base_new(Faction faction, Vec2 pos)
{
	return (Base){
		.faction = faction,
		.health = 100,
		.pos = pos,
		.size = 50,
		// .radius = 5,
	};
}

void base_update(Base *base)
{
	base->health += 1;
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
