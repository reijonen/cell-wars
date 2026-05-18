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

typedef enum ShaderType
{
	VERTEX_SHADER = SDL_GPU_SHADERSTAGE_VERTEX,
	FRAGMENT_SHADER = SDL_GPU_SHADERSTAGE_FRAGMENT,
} ShaderType;

typedef enum PrimitiveType
{
	TRIANGLE_PRIMITIVE = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
	LINE_PRIMITIVE = SDL_GPU_PRIMITIVETYPE_LINELIST,
} PrimitiveType;

Renderer renderer_new(SDL_Window *wnd);
void renderer_release(Renderer *renderer);
void renderer_begin_frame(Renderer *renderer, SDL_Window *wnd);
void renderer_end_frame(Renderer *renderer);

void renderer_shader_new(Renderer *renderer, const char *path, const ShaderType type);
void renderer_buffer_new(Renderer *renderer);
void renderer_pipeline_new(Renderer *renderer, SDL_Window *wnd, const PrimitiveType type);