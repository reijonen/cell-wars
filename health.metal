#include <metal_stdlib>
using namespace metal;

#define BASE_COUNT 5
#define HEALTH_DOT_SIZE 4.0
#define HEALTH_DOT_GAP 2.0
#define HEALTH_RING_SPACING (HEALTH_DOT_SIZE + HEALTH_DOT_GAP)

struct VertexIn
{
	float2 pos [[attribute(0)]];
	half4 color [[attribute(1)]];
};

struct VertexOut
{
	float4 pos [[position]];
	half4 color;
	float pointSize [[point_size]];
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

static float2 resolve_ring_slot_position(float2 center, float baseSize, uint slot)
{
	const float pitch = HEALTH_DOT_SIZE + HEALTH_DOT_GAP;
	float2 baseTopLeft = float2(center.x - (baseSize / 2.0), center.y + (baseSize / 2.0));

	uint ring = 0u;
	uint ringIndex = slot;
	while (true)
	{
		float ringOffset = HEALTH_RING_SPACING * float(ring + 1u);
		float ringSide = baseSize + ringOffset * 2.0;
		uint sideSlots = max(2u, uint(floor(ringSide / pitch)) + 1u);
		uint perimeterSlots = sideSlots * 4u - 4u;

		if (ringIndex < perimeterSlots)
		{
			float2 ringTopLeft = float2(baseTopLeft.x - ringOffset, baseTopLeft.y + ringOffset);
			float sideSpan = pitch * float(sideSlots - 1u);
			float left = ringTopLeft.x;
			float right = ringTopLeft.x + sideSpan;
			float top = ringTopLeft.y;
			float bottom = ringTopLeft.y - sideSpan;

			float2 dotCenter = ringTopLeft;
			if (ringIndex < sideSlots)
			{
				dotCenter = float2(left + float(ringIndex) * pitch, top);
			}
			else if (ringIndex < sideSlots + (sideSlots - 1u))
			{
				uint j = ringIndex - sideSlots + 1u;
				dotCenter = float2(right, top - float(j) * pitch);
			}
			else if (ringIndex < sideSlots + 2u * (sideSlots - 1u))
			{
				uint j = ringIndex - (sideSlots + (sideSlots - 1u)) + 1u;
				dotCenter = float2(right - float(j) * pitch, bottom);
			}
			else
			{
				uint j = ringIndex - (sideSlots + 2u * (sideSlots - 1u)) + 1u;
				dotCenter = float2(left, bottom + float(j) * pitch);
			}

			return dotCenter;
		}

		ringIndex -= perimeterSlots;
		ring++;
	}
}

VertexOut vertex vertexMain(
	VertexIn in [[stage_in]],
	constant float4x4 &projection [[buffer(0)]],
	constant uint *healths [[buffer(2)]],
	constant uint *factions [[buffer(3)]],
	uint instanceId [[instance_id]],
	uint vertexId [[vertex_id]])
{
	VertexOut out;

	uint baseIndex = vertexId;
	if (baseIndex >= BASE_COUNT || instanceId >= healths[baseIndex])
	{
		out.pos = projection * float4(-10000.0, -10000.0, 0.0, 1.0);
		out.color = half4(0.0, 0.0, 0.0, 0.0);
		out.pointSize = 1.0;
		return out;
	}

	float2 dotCenter = resolve_ring_slot_position(in.pos, 50.0, instanceId);
	out.pos = projection * float4(dotCenter, 0.0, 1.0);
	out.color = faction_color(factions[baseIndex]);
	out.pointSize = HEALTH_DOT_SIZE;
	return out;
}

half4 fragment fragmentMain(
	VertexOut in [[stage_in]],
	float2 pointCoord [[point_coord]])
{
	float2 centeredCoord = pointCoord - 0.5;
	if (length(centeredCoord) > 0.5)
	{
		discard_fragment();
	}

	return in.color;
}
