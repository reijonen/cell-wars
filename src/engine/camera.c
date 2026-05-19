#include "camera.h"

Camera camera_init(unsigned world_width, unsigned world_height)
{
	Camera c = {0};

	c.projection[0][0] = 2.0f / world_width;
	c.projection[0][1] = 0.0f;
	c.projection[0][2] = 0.0f;
	c.projection[0][3] = 0.0f;

	c.projection[1][0] = 0.0f;
	c.projection[1][1] = 2.0f / world_height;
	c.projection[1][2] = 0.0f;
	c.projection[1][3] = 0.0f;

	c.projection[2][0] = 0.0f;
	c.projection[2][1] = 0.0f;
	c.projection[2][2] = 1.0f;
	c.projection[2][3] = 0.0f;

	c.projection[3][0] = -1.0f;
	c.projection[3][1] = -1.0f;
	c.projection[3][2] = 0.0f;
	c.projection[3][3] = 1.0f;

	return c;
}