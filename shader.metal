#include <metal_stdlib>
using namespace metal;

struct VertexIn {
	float2 pos [[attribute(0)]];
};

struct VertexOut {
	float4 pos [[position]];
};

VertexOut vertex vertexMain(
	VertexIn in [[stage_in]]
) {
	VertexOut out;
	out.pos = float4(in.pos, 0.0, 1.0);

	return out;
}

half4 fragment fragmentMain( 
    VertexOut in [[stage_in]]
) {
    return half4(1.0, 1.0, 1.0, 1.0);
}