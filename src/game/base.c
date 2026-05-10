#include "base.h"

struct Base base_new()
{
	return (struct Base){
		.faction = NEUTRAL,
		.health = 100,
		.pos = {
			.x = 50,
			.y = 50},
		.radius = 5};
}

void base_update(struct Base *base)
{
	base->health += 1;
}

void base_take_damage(struct Base *base, enum Faction from)
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
			base->faction = NEUTRAL;
		}
	}
}
