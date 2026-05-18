#include <stdlib.h>

#include "renderer.h"

typedef struct Vertex
{
	float x;
	float y;
} Vertex;

Vertex verts[] = {
	{0.0, 0.5},
	{0.5, -0.5},
	{-0.5, -0.5},
};

Renderer renderer_new(SDL_Window *wnd)
{
	SDL_GPUDevice *device = SDL_CreateGPUDevice(
		SDL_GPU_SHADERFORMAT_MSL,
		true,
		"metal");
	if (!device)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	bool success = SDL_ClaimWindowForGPUDevice(device, wnd);
	if (!success)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to claim window for gpu: %s\n", SDL_GetError());
		exit(1);
	}

	return (Renderer){
		.device = device};
}

void renderer_release(Renderer *renderer)
{
	SDL_DestroyGPUDevice(renderer->device);
	// TODO: muut?
}

void renderer_shader_new(Renderer *renderer, const char *path, const ShaderType type)
{
	size_t shaderSize;
	void *shaderSrc = SDL_LoadFile(path, &shaderSize);
	printf("shaderSrc:\n%s\n\n", shaderSrc);

	// SDL_GPUShaderCreateInfo ctx = {
	// 	.code = (Uint8 *)shaderSrc,
	// 	.code_size = shaderSize,
	// 	.entrypoint = type == 0 ? "vertexMain" : "fragmentMain",
	// 	.format = SDL_GPU_SHADERFORMAT_MSL,
	// 	.stage = (SDL_GPUShaderStage)type,
	// 	.num_samplers = 0,
	// 	.num_storage_textures = 0,
	// 	.num_storage_buffers = 0,
	// 	.num_uniform_buffers = 0,
	// 	.props = 0};

	// SDL_GPUShader *shader = SDL_CreateGPUShader(
	// 	renderer->device,
	// 	&ctx);

	// if (type == VERTEX_SHADER)
	// {
	// 	renderer->vertexShader = shader;
	// }
	// else if (type == FRAGMENT_SHADER)
	// {
	// 	renderer->fragmentShader = shader;
	// }

	SDL_GPUShaderCreateInfo vertCtx = {0};
	vertCtx.code = (Uint8 *)shaderSrc,
	vertCtx.code_size = shaderSize,
	vertCtx.entrypoint = "vertexMain",
	vertCtx.format = SDL_GPU_SHADERFORMAT_MSL,
	vertCtx.stage = SDL_GPU_SHADERSTAGE_VERTEX,

	renderer->vertexShader = SDL_CreateGPUShader(
		renderer->device,
		&vertCtx);

	SDL_GPUShaderCreateInfo fragCtx = {0};
	fragCtx.code = (Uint8 *)shaderSrc,
	fragCtx.code_size = shaderSize,
	fragCtx.entrypoint = "fragmentMain",
	fragCtx.format = SDL_GPU_SHADERFORMAT_MSL,
	fragCtx.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,

	renderer->fragmentShader = SDL_CreateGPUShader(
		renderer->device,
		&fragCtx);

	SDL_free(shaderSrc);
}

void renderer_buffer_new(Renderer *renderer)
{
}

void renderer_create_buffer(Renderer *renderer)
{
	const SDL_GPUBufferCreateInfo ctx = {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = sizeof(verts),
		.props = 0};

	renderer->vertexBuffer = SDL_CreateGPUBuffer(renderer->device, &ctx);

	// TODO: release gpu buff
}

void renderer_upload_buffer(Renderer *renderer)
{

	SDL_GPUTransferBufferCreateInfo ctx = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = sizeof(verts),
		.props = 0};
	SDL_GPUTransferBuffer *tb = SDL_CreateGPUTransferBuffer(renderer->device, &ctx);

	float *data = SDL_MapGPUTransferBuffer(renderer->device, tb, false);
	SDL_memcpy(data, verts, sizeof(verts));
	// TODO: SDL_UnmapGPUTransferBuffer(renderer->device, tb);

	renderer->commandBuffer = SDL_AcquireGPUCommandBuffer(renderer->device);
	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(renderer->commandBuffer);

	SDL_GPUTransferBufferLocation source = {
		.transfer_buffer = tb,
		.offset = 0};

	SDL_GPUBufferRegion dest = {
		.buffer = renderer->vertexBuffer,
		.offset = 0,
		.size = sizeof(verts)};

	SDL_UploadToGPUBuffer(copy_pass, &source, &dest, false);

	SDL_EndGPUCopyPass(copy_pass);

	SDL_SubmitGPUCommandBuffer(renderer->commandBuffer);
}

void renderer_pipeline_new(Renderer *renderer, SDL_Window *wnd, const PrimitiveType type)
{
	SDL_GPUGraphicsPipelineCreateInfo pci = {
		// TODO: .primitive_type = (SDL_GPUPrimitiveType)type,
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.vertex_shader = renderer->vertexShader,
		.fragment_shader = renderer->fragmentShader,
		.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL};

	// TODO:
	// SDL_ReleaseGPUShader(renderer->device, renderer->vertexShader);
	// SDL_ReleaseGPUShader(renderer->device, renderer->fragmentShader);

	SDL_GPUVertexBufferDescription vbDec[1] = {0};
	vbDec[0].slot = 0;
	vbDec[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vbDec[0].instance_step_rate = 0;
	vbDec[0].pitch = sizeof(Vertex);

	pci.vertex_input_state.num_vertex_buffers = 1;
	pci.vertex_input_state.vertex_buffer_descriptions = vbDec;

	SDL_GPUVertexAttribute vattrib[1] = {0};
	vattrib[0].buffer_slot = 0;
	vattrib[0].location = 0;
	vattrib[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	vattrib[0].offset = 0;

	pci.vertex_input_state.num_vertex_attributes = 1;
	pci.vertex_input_state.vertex_attributes = vattrib;

	SDL_GPUColorTargetDescription colorTargetDescriptions[1] = {0};
	colorTargetDescriptions[0].blend_state.enable_blend = true;
	colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(renderer->device, wnd);

	pci.target_info.num_color_targets = 1;
	pci.target_info.color_target_descriptions = colorTargetDescriptions;

	renderer->pipeline = SDL_CreateGPUGraphicsPipeline(renderer->device, &pci);
	if (!renderer->pipeline)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create graphics pipeline: %s\n", SDL_GetError());
		exit(1);
	}

	// TODO: temp
	renderer_create_buffer(renderer);

	// TODO: temp
	// requires command buffer
	renderer_upload_buffer(renderer);
}

void renderer_begin_frame(Renderer *renderer, SDL_Window *wnd)
{
	renderer->commandBuffer = SDL_AcquireGPUCommandBuffer(renderer->device);

	SDL_GPUTexture *swapchain;
	Uint32 width, height;
	SDL_WaitAndAcquireGPUSwapchainTexture(renderer->commandBuffer, wnd, &swapchain, &width, &height);
	if (!swapchain)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to acquire swapchain: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_GPUColorTargetInfo cti = {0};
	cti.texture = swapchain;
	cti.clear_color = (SDL_FColor){0.0f, 0.0f, 0.0f, 1.0f};
	cti.load_op = SDL_GPU_LOADOP_CLEAR;
	cti.store_op = SDL_GPU_STOREOP_STORE;

	// SDL_SubmitGPUCommandBuffer(renderer->renderPass);

	renderer->renderPass = SDL_BeginGPURenderPass(renderer->commandBuffer, &cti, 1, NULL);

	SDL_BindGPUGraphicsPipeline(renderer->renderPass, renderer->pipeline);

	SDL_GPUBufferBinding vbindings[1];
	vbindings[0].buffer = renderer->vertexBuffer;
	vbindings[0].offset = 0;

	SDL_BindGPUVertexBuffers(renderer->renderPass, 0, vbindings, 1);
}

// TODO: create ja update uniform
// tai indexed rendering

void renderer_end_frame(Renderer *renderer)
{
	// TODO: nää pitää varmaan jotenkin definee Game:ssa kun uploadaa buffereita - ja ehkä pitää muutenkin controlloida jos on erilaisia objekteja, esim text
	SDL_DrawGPUPrimitives(renderer->renderPass, 3, 1, 0, 0);
	SDL_EndGPURenderPass(renderer->renderPass);
	SDL_SubmitGPUCommandBuffer(renderer->commandBuffer);
}