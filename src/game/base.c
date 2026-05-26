#include "base.h"
#include "game_render_ids.h"
#include "engine/renderer.h"

#include <stdio.h>

void base_init(Base *b, Faction faction, Vec2 pos, unsigned size)
{
	b->faction = faction;
	b->health = 10;
	b->regen_accum_seconds = 0.0;
	b->pos = pos;
	b->size = size;
}

void base_update(Base *base, double delta_time)
{
	base->regen_accum_seconds += delta_time;

	while (base->regen_accum_seconds >= 1.0)
	{
		if (base->faction != NEUTRAL_FACTION)
		{
			base->health += 1;
		}
		base->regen_accum_seconds -= 1.0;
	}
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

void base_render(uint32_t *factions, unsigned base_count)
{
	renderer_update_uniform(BASE_FACTIONS_UNIFORM_IDX, factions, sizeof(uint32_t) * base_count);
	renderer_draw(SHAPE_PIPELINE_IDX, SHAPE_BUFF_IDX, base_count * 6, 1);
}

void base_health_render(uint32_t *healths, uint32_t *factions, unsigned base_count, unsigned max_health)
{
	renderer_update_uniform(HEALTHS_UNIFORM_IDX, healths, sizeof(uint32_t) * base_count);
	renderer_update_uniform(HEALTH_FACTIONS_UNIFORM_IDX, factions, sizeof(uint32_t) * base_count);
	renderer_draw(HEALTH_PIPELINE_IDX, HEALTH_BUFF_IDX, base_count, max_health);
}
