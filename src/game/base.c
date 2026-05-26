#include "base.h"
#include "game_render_ids.h"
#include "engine/renderer.h"

#include <stdio.h>

static void generate_quad(Vertex *v, Vec2 top_left, unsigned size, Faction faction)
{
	Color color = {255};
	color.a = 255;
	if (faction == RED_FACTION)
	{
		color.r = 255;
		color.g = 0;
		color.b = 0;
	}
	else if (faction == BLUE_FACTION)
	{
		color.r = 0;
		color.g = 0;
		color.b = 255;
	}
	else
	{
		color.r = 127;
		color.g = 127;
		color.b = 127;
	}

	v[0].pos.x = top_left.x;
	v[0].pos.y = top_left.y;
	v[0].color = color;
	v[1].pos.x = top_left.x + size;
	v[1].pos.y = top_left.y;
	v[1].color = color;
	v[2].pos.x = top_left.x;
	v[2].pos.y = top_left.y - size;
	v[2].color = color;
	v[3].pos.x = top_left.x + size;
	v[3].pos.y = top_left.y;
	v[3].color = color;
	v[4].pos.x = top_left.x + size;
	v[4].pos.y = top_left.y - size;
	v[4].color = color;
	v[5].pos.x = top_left.x;
	v[5].pos.y = top_left.y - size;
	v[5].color = color;
}

void base_init(Base *b, Faction faction, Vec2 pos, unsigned size)
{
	b->faction = faction;
	b->health = 10;
	b->regen_accum_seconds = 0.0;
	b->pos = pos;
	b->size = size;
}

void bases_init_default(Base *bases, unsigned window_width, unsigned window_height, unsigned base_size)
{
	base_init(
		&bases[0],
		BLUE_FACTION,
		(Vec2){
			(window_width / 2) - (base_size / 2),
			200 + (base_size / 2)},
		base_size);

	base_init(
		&bases[4],
		RED_FACTION,
		(Vec2){
			(window_width / 2) - (base_size / 2),
			(window_height - 200) + (base_size / 2)},
		base_size);

	base_init(
		&bases[1],
		NEUTRAL_FACTION,
		(Vec2){
			(window_width / 3) - (base_size / 2),
			(window_height / 2) + (base_size / 2)},
		base_size);

	base_init(
		&bases[2],
		NEUTRAL_FACTION,
		(Vec2){
			(window_width / 2) - (base_size / 2),
			(window_height / 2) + (base_size / 2)},
		base_size);

	base_init(
		&bases[3],
		NEUTRAL_FACTION,
		(Vec2){
			(window_width / 3 * 2) - (base_size / 2),
			(window_height / 2) + (base_size / 2)},
		base_size);
}

void base_build_vertices(Vertex *vertices, const Base *bases, unsigned base_count)
{
	for (unsigned i = 0; i < base_count; i++)
	{
		generate_quad(vertices + (i * 6), bases[i].pos, bases[i].size, bases[i].faction);
	}
}

void base_health_build_vertices(Vertex *vertices, const Base *bases, unsigned base_count)
{
	for (unsigned i = 0; i < base_count; i++)
	{
		vertices[i].pos = (Vec2){
			bases[i].pos.x + bases[i].size / 2,
			bases[i].pos.y - bases[i].size / 2};
		vertices[i].color = (Color){255, 255, 255, 255};
	}
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
