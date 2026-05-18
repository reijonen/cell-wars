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

Renderer renderer_new(SDL_Window *wnd);
void renderer_release(Renderer *renderer, SDL_Window *wnd);
void renderer_begin_frame(Renderer *renderer, SDL_Window *wnd);
void renderer_end_frame(Renderer *renderer);

void renderer_shader_new(Renderer *renderer, const char *path);
void renderer_buffer_new(Renderer *renderer, void *vertices, unsigned vertex_bytes, size_t vertex_count);
void renderer_pipeline_new(Renderer *renderer, SDL_Window *wnd, unsigned stride, const PrimitiveType type);