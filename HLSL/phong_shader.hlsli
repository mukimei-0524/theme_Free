
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 world_position : POSITION;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world;
	float4 ka;
	float4 kd;
	float4 ks;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
	row_major float4x4 view_projection;
	float4 light_direction;
	float4 camera_position;
};

//cbuffer LIGHT_CONSTANT_BUFFER : register(b2)
//{
//	float4 ambient_color;
//	float4 directional_light_direction;
//	float4 directional_light_color;
//	/*point_lights point_light[8];
//	spot_lights spot_light[8];*/
//};

cbuffer HEMISPHERE_LIGHT_CONSTANT_BUFFER : register(b4)
{
	float4 sky_color;
	float4 ground_color;
	float4 hemisphere_weight;
};


cbuffer FOG_CONSTNT_BUFFER:register(b5)
{
	float4 fog_color;
	float4 fog_range;
}


#include "shading_functions.hlsli"