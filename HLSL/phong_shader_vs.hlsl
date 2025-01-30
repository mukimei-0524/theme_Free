
#include "phong_shader.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
	VS_OUT vout = (VS_OUT)0;
	vout.position = mul(position, mul(world, view_projection));

	vout.world_position = mul(position, world);
	vout.normal = normalize(mul(float4(normal.xyz, 0), world)).xyz;
	vout.binormal = float3(0.0f, 1.0f, 0.001f);	//	���̏�x�N�g��
	vout.binormal = normalize(vout.binormal);
	vout.tangent = normalize(cross(vout.binormal, vout.normal));	//�]�@��(Y)�Ɩ@��(Z)�̊O��
	vout.binormal = normalize(cross(vout.normal, vout.tangent));	//�@��(Z)�Ɛڐ�(X)�̊O��
	vout.texcoord = texcoord;

	return vout;
}
