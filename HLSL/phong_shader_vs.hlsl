
#include "phong_shader.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
	VS_OUT vout = (VS_OUT)0;
	vout.position = mul(position, mul(world, view_projection));

	vout.world_position = mul(position, world);
	vout.normal = normalize(mul(float4(normal.xyz, 0), world)).xyz;
	vout.binormal = float3(0.0f, 1.0f, 0.001f);	//	仮の上ベクトル
	vout.binormal = normalize(vout.binormal);
	vout.tangent = normalize(cross(vout.binormal, vout.normal));	//従法線(Y)と法線(Z)の外積
	vout.binormal = normalize(cross(vout.normal, vout.tangent));	//法線(Z)と接線(X)の外積
	vout.texcoord = texcoord;

	return vout;
}
