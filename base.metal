#include <metal_stdlib>
using namespace metal;

struct VertexIn
{
	float2 pos [[attribute(0)]];
	half4 color [[attribute(1)]];
};

struct VertexOut
{
	float4 pos [[position]];
	half4 color;
};

static half4 faction_color(uint faction)
{
	if (faction == 2u)
	{
		return half4(1.0, 0.0, 0.0, 1.0);
	}
	if (faction == 1u)
	{
		return half4(0.0, 0.0, 1.0, 1.0);
	}
	return half4(0.5, 0.5, 0.5, 1.0);
}

VertexOut vertex vertexMain(
	VertexIn in [[stage_in]],
	constant float4x4 &projection [[buffer(0)]],
	constant uint *baseFactions [[buffer(2)]],
	uint vertexId [[vertex_id]])
{
	VertexOut out;
	out.pos = projection * float4(in.pos.x, in.pos.y, 0.0, 1.0);
	out.color = faction_color(baseFactions[vertexId / 6u]);
	return out;
}

half4 fragment fragmentMain(VertexOut in [[stage_in]])
{
	return in.color;
}
