#pragma once

#include <SDL3/SDL.h>

#define PIPELINE_COUNT 5
#define SHADERS_PER_PIPELINE_COUNT 2
#define BUFFER_COUNT 5

typedef struct Renderer
{
	SDL_GPUDevice *device;

	SDL_GPUCommandBuffer *commandBuffer;

	SDL_GPURenderPass *renderPass;

	SDL_GPUGraphicsPipeline *pipelines[PIPELINE_COUNT];

	SDL_GPUShader *shaders[PIPELINE_COUNT][SHADERS_PER_PIPELINE_COUNT];

	SDL_GPUBuffer *buffers[BUFFER_COUNT];
} Renderer;

typedef enum PrimitiveType
{
	TRIANGLE_PRIMITIVE = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
	LINE_PRIMITIVE = SDL_GPU_PRIMITIVETYPE_LINELIST,
	POINT_PRIMITIVE = SDL_GPU_PRIMITIVETYPE_POINTLIST,
} PrimitiveType;

typedef enum ShaderType
{
	VERTEX_SHADER = SDL_GPU_SHADERSTAGE_VERTEX,
	FRAGMENT_SHADER = SDL_GPU_SHADERSTAGE_FRAGMENT,
} ShaderType;

void renderer_init(SDL_Window *wnd);
void renderer_release();
void renderer_begin_frame();
void renderer_end_frame();

void renderer_shader_new(unsigned char pipeline_idx, char *path, ShaderType type, unsigned char uniform_count);
void renderer_buffer_new(unsigned index, void *vertices, unsigned vertex_bytes, size_t vertex_count);
void renderer_pipeline_new(unsigned index, unsigned stride, unsigned offset, const PrimitiveType type);
void renderer_update_uniform(unsigned index, void *data, size_t bytes);
void renderer_draw(unsigned pipeline_index, unsigned buffer_index, unsigned vertex_count, unsigned instance_count);
