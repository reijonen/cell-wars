#include <metal_stdlib>
using namespace metal;

struct VertexIn {
	float2 pos [[attribute(0)]];
	half4 color [[attribute(1)]];
};

struct VertexOut {
	float4 pos [[position]];
	half4 color;
};

// For MSL/metallib, use the following order:
// [[texture]]: Sampled textures, followed by storage textures
// [[sampler]]: Samplers with indices corresponding to the sampled textures
// [[buffer]]: Uniform buffers, followed by storage buffers. Vertex buffer 0 is bound at [[buffer(14)]], vertex buffer 1 at [[buffer(15)]], and so on. Rather than manually authoring vertex buffer indices, use the [[stage_in]] attribute which will automatically use the vertex input information from the SDL_GPUGraphicsPipeline.


VertexOut vertex vertexMain(
	VertexIn in [[stage_in]],
	constant float4x4& projection [[buffer(0)]]
) {
	VertexOut out;
	
	out.pos = projection * float4(in.pos.x, in.pos.y, 0.0, 1.0);
	out.color = in.color;

	return out;
}

half4 fragment fragmentMain( 
    VertexOut in [[stage_in]]
) {
    return in.color;
}