#pragma once

#include "types/mat4x4.h"

typedef struct Camera
{
	mat4x4 projection;
} Camera;

Camera camera_init(unsigned world_width, unsigned world_height);