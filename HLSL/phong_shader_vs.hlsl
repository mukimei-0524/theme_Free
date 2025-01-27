#include "SkinnedMesh.hlsli"
VS_OUT main(VS_IN vin)
{
    vin.normal.w = 0;

    float4 blended_position = { 0, 0, 0, 1 };
    float4 blended_normal = { 0, 0, 0, 0 };
    for (int bone_index = 0; bone_index < 4; ++bone_index)
    {
        blended_position += vin.bone_weights[bone_index]
            * mul(vin.position, bone_transforms[vin.bone_indices[bone_index]]);
        blended_normal += vin.bone_weights[bone_index]
            * mul(vin.normal, bone_transforms[vin.bone_indices[bone_index]]);
    }
    vin.position = float4(blended_position.xyz, 1.0f);
    vin.normal = float4(blended_normal.xyz, 0.0f);

    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, view_projection));
    vin.normal.w = 0;
    vout.world_normal = normalize(mul(vin.normal, world));

    vout.world_position = mul(vin.position, world);

    vout.texcoord = vin.texcoord;
    vout.color.rgb = material_color.rgb;
    vout.color.a = material_color.a;

    return vout;
}

#if 0
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
#endif