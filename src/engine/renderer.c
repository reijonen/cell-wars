#include <stdlib.h>

#include "renderer.h"

static Renderer renderer;

// TODO: tmp
SDL_Window *window = NULL;

void renderer_init(SDL_Window *wnd)
{
	window = wnd;

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

	renderer.device = device;
}

void renderer_release()
{
	SDL_ReleaseGPUShader(renderer.device, renderer.vertexShader);
	SDL_ReleaseGPUShader(renderer.device, renderer.fragmentShader);
	SDL_ReleaseGPUBuffer(renderer.device, renderer.vertexBuffer);
	SDL_DestroyGPUDevice(renderer.device);
	// TODO: muut?
}

void renderer_shader_new(const char *path)
{
	size_t shaderSize;
	void *shaderSrc = SDL_LoadFile(path, &shaderSize);

	SDL_GPUShaderCreateInfo vertCtx = {0};
	vertCtx.code = (Uint8 *)shaderSrc,
	vertCtx.code_size = shaderSize,
	vertCtx.entrypoint = "vertexMain",
	vertCtx.format = SDL_GPU_SHADERFORMAT_MSL,
	vertCtx.stage = SDL_GPU_SHADERSTAGE_VERTEX,
	vertCtx.num_uniform_buffers = 1;
	renderer.vertexShader = SDL_CreateGPUShader(renderer.device, &vertCtx);

	SDL_GPUShaderCreateInfo fragCtx = {0};
	fragCtx.code = (Uint8 *)shaderSrc,
	fragCtx.code_size = shaderSize,
	fragCtx.entrypoint = "fragmentMain",
	fragCtx.format = SDL_GPU_SHADERFORMAT_MSL,
	fragCtx.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
	renderer.fragmentShader = SDL_CreateGPUShader(renderer.device, &fragCtx);

	SDL_free(shaderSrc);
}

void renderer_buffer_new(void *vertices, unsigned vertex_bytes, size_t vertex_count)
{
	size_t vertexBytes = vertex_bytes * vertex_count;

	SDL_GPUBufferCreateInfo bci = {0};
	bci.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	bci.size = vertexBytes;
	renderer.vertexBuffer = SDL_CreateGPUBuffer(renderer.device, &bci);

	// TODO: release gpu buff

	SDL_GPUTransferBufferCreateInfo tbci = {0};
	tbci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	tbci.size = vertexBytes;
	SDL_GPUTransferBuffer *tb = SDL_CreateGPUTransferBuffer(renderer.device, &tbci);

	float *data = SDL_MapGPUTransferBuffer(renderer.device, tb, false);
	SDL_memcpy(data, vertices, vertexBytes);
	SDL_UnmapGPUTransferBuffer(renderer.device, tb);

	SDL_GPUTransferBufferLocation source = {0};
	source.transfer_buffer = tb;
	source.offset = 0;

	SDL_GPUBufferRegion dest = {0};
	dest.buffer = renderer.vertexBuffer;
	dest.size = vertexBytes;
	dest.offset = 0;

	renderer.commandBuffer = SDL_AcquireGPUCommandBuffer(renderer.device);
	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(renderer.commandBuffer);

	SDL_UploadToGPUBuffer(copy_pass, &source, &dest, false);

	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(renderer.commandBuffer);
}

void renderer_pipeline_new(unsigned stride, unsigned offset, const PrimitiveType type)
{
	SDL_GPUGraphicsPipelineCreateInfo pci = {
		.primitive_type = (SDL_GPUPrimitiveType)type,
		.vertex_shader = renderer.vertexShader,
		.fragment_shader = renderer.fragmentShader,
		.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL};

	SDL_GPUVertexBufferDescription vbDec[1] = {0};
	vbDec[0].slot = 0;
	vbDec[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vbDec[0].instance_step_rate = 0;
	vbDec[0].pitch = stride;

	pci.vertex_input_state.num_vertex_buffers = 1;
	pci.vertex_input_state.vertex_buffer_descriptions = vbDec;

	SDL_GPUVertexAttribute vattrib[2] = {0};
	vattrib[0].buffer_slot = 0;
	vattrib[0].location = 0;
	vattrib[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	vattrib[0].offset = 0;

	vattrib[1].buffer_slot = 0;
	vattrib[1].location = 1;
	vattrib[1].format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM;
	vattrib[1].offset = offset;

	pci.vertex_input_state.num_vertex_attributes = 2;
	pci.vertex_input_state.vertex_attributes = vattrib;

	SDL_GPUColorTargetDescription colorTargetDescriptions[1] = {0};
	colorTargetDescriptions[0].blend_state.enable_blend = true;
	colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(renderer.device, window);

	pci.target_info.num_color_targets = 1;
	pci.target_info.color_target_descriptions = colorTargetDescriptions;

	renderer.pipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pci);
	if (!renderer.pipeline)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create graphics pipeline: %s\n", SDL_GetError());
		exit(1);
	}
}

void renderer_update_uniform(void *data, size_t bytes)
{
	renderer.commandBuffer = SDL_AcquireGPUCommandBuffer(renderer.device);

	SDL_PushGPUVertexUniformData(renderer.commandBuffer, 0, data, bytes);

	SDL_SubmitGPUCommandBuffer(renderer.commandBuffer);
}

void renderer_begin_frame()
{
	renderer.commandBuffer = SDL_AcquireGPUCommandBuffer(renderer.device);

	SDL_GPUTexture *swapchain;
	Uint32 width, height;
	SDL_WaitAndAcquireGPUSwapchainTexture(renderer.commandBuffer, window, &swapchain, &width, &height);
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

	renderer.renderPass = SDL_BeginGPURenderPass(renderer.commandBuffer, &cti, 1, NULL);

	SDL_BindGPUGraphicsPipeline(renderer.renderPass, renderer.pipeline);

	SDL_GPUBufferBinding vbindings[1];
	vbindings[0].buffer = renderer.vertexBuffer;
	vbindings[0].offset = 0;

	SDL_BindGPUVertexBuffers(renderer.renderPass, 0, vbindings, 1);
}

// TODO: create ja update uniform
// tai indexed rendering

void renderer_end_frame()
{
	// TODO: nää pitää varmaan jotenkin definee Game:ssa kun uploadaa buffereita - ja ehkä pitää muutenkin controlloida jos on erilaisia objekteja, esim text
	// TODO: miten tehdä dynaamisesti
	SDL_DrawGPUPrimitives(renderer.renderPass, 18, 1, 0, 0);
	SDL_EndGPURenderPass(renderer.renderPass);
	SDL_SubmitGPUCommandBuffer(renderer.commandBuffer);
}