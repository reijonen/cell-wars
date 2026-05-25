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
	constant uint& fleetSize [[buffer(2)]],
	uint instanceId [[instance_id]],
	uint vertexId [[vertex_id]]
) {
	VertexOut out;

	float2 top_center = float2(test.x, test.y);
	float2 direction = float2(test.z, test.w);
	
	float2 perpendicular = float2(-direction.y, direction.x);
	uint depth = 0u;
	uint cell_count = 1u;
	while (cell_count < max(fleetSize, 1u))
	{
		cell_count *= 3u;
		depth++;
	}

	uint idx = instanceId;
	float2 frac = float2(0.0, 0.0);
	float scale = 1.0;
	for (uint i = 0u; i < depth; i++)
	{
		uint digit = idx % 3u;
		idx /= 3u;

		if (digit == 1u)
		{
			frac += float2(scale * 0.5, 0.0);
		}
		else if (digit == 2u)
		{
			frac += float2(scale * 0.25, scale * 0.4330127);
		}
		scale *= 0.5;
	}

	float spread = 36.0;
	float2 centered = frac - float2(0.25, 0.14433756);
	float2 fractal_offset = centered * spread;
	float2 world_center = top_center + (perpendicular * fractal_offset.x) + (direction * fractal_offset.y);

	float tri_len = 6.0;
	float tri_half_w = 3.0;
	float2 bottom_center = world_center - direction * tri_len;
	float2 bottom_left = bottom_center + perpendicular * tri_half_w;
	float2 bottom_right = bottom_center + -perpendicular * tri_half_w;

	if(vertexId == 0){
		out.pos = projection * float4(world_center.x, world_center.y, 0.0, 1.0);
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
