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
	// float pointSize [[point_size]];

// For MSL/metallib, use the following order:
// [[texture]]: Sampled textures, followed by storage textures
// [[sampler]]: Samplers with indices corresponding to the sampled textures
// [[buffer]]: Uniform buffers, followed by storage buffers. Vertex buffer 0 is bound at [[buffer(14)]], vertex buffer 1 at [[buffer(15)]], and so on. Rather than manually authoring vertex buffer indices, use the [[stage_in]] attribute which will automatically use the vertex input information from the SDL_GPUGraphicsPipeline.


VertexOut vertex vertexMain(
	VertexIn in [[stage_in]],
	constant float4x4& projection [[buffer(0)]],
	constant float4& test [[buffer(1)]],
	uint instanceId [[instance_id]],
	uint vertexId [[vertex_id]]
) {
	VertexOut out;

	float2 top_center = float2(test.x, test.y);
	float2 direction = float2(test.z, test.w);
	
	float2 perpendicular = float2(-direction.y, direction.x);

	float2 bottom_center = top_center + -direction * 25;
	
	float2 bottom_left = bottom_center + perpendicular * 12.5;
	float2 bottom_right = bottom_center + -perpendicular * 12.5;

	if(vertexId == 0){
		out.pos = projection * float4(top_center.x, top_center.y, 0.0, 1.0);
	} else if (vertexId == 1){
		out.pos = projection * float4(bottom_right.x, bottom_right.y, 0.0, 1.0);
	} else if (vertexId == 2){
		out.pos = projection * float4(bottom_left.x, bottom_left.y, 0.0, 1.0);
	}

	out.color = in.color;

	return out;
}

	// float2 pointCoord [[point_coord]]
half4 fragment fragmentMain( 
    VertexOut in [[stage_in]]
) {
	// float2 centeredCoord = pointCoord - 0.5;
    
    // float dist = length(centeredCoord);
    
    // if (dist > 0.5) {
    //     discard_fragment();
    // }

    return in.color;
}