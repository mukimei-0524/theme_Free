
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 world_position : POSITION;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

cbuffer FOG_CONSTNT_BUFFER:register(b0)
{
	float4 fog_color;
	float4 fog_range;
}

#include "shading_functions.hlsli"