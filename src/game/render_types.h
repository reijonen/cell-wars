#pragma once

#include "../engine/types/vec2.h"

typedef struct Color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Color;

typedef struct Vertex
{
	Vec2 pos;
	Color color;
} Vertex;
