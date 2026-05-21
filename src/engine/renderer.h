#pragma once

#include <SDL3/SDL.h>

typedef struct Renderer
{
	SDL_GPUDevice *device;
	SDL_GPUShader *vertexShader;
	SDL_GPUShader *fragmentShader;
	SDL_GPUBuffer *vertexBuffer;
	SDL_GPUCommandBuffer *commandBuffer;
	SDL_GPURenderPass *renderPass;
	SDL_GPUGraphicsPipeline *pipeline;
} Renderer;

typedef enum PrimitiveType
{
	TRIANGLE_PRIMITIVE = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
	LINE_PRIMITIVE = SDL_GPU_PRIMITIVETYPE_LINELIST,
} PrimitiveType;

void renderer_init(SDL_Window *wnd);
void renderer_release();
void renderer_begin_frame();
void renderer_end_frame();

void renderer_shader_new(const char *path);
void renderer_buffer_new(void *vertices, unsigned vertex_bytes, size_t vertex_count);
void renderer_pipeline_new(unsigned stride, unsigned offset, const PrimitiveType type);
void renderer_update_uniform(void *data, size_t bytes);