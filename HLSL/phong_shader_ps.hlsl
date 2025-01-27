

#include "SkinnedMesh.hlsli"

SamplerState samplerState : register(s0);
Texture2D textureMap : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = textureMap.Sample(samplerState, pin.texcoord);
	float alpha = color.a;

	float3 E = normalize(pin.world_position.xyz - camera_position.xyz);

	float3 N = normalize(pin.world_normal.xyz);
	float3 L = normalize(-light_direction.xyz);

	float3 diffuse = color.rgb * max(0, dot(N, L) * 0.5f + 0.5f);
	float3 specular = 0; // TODO	スペキュラの計算を見直す
	float3 ambient = color.rgb * 0.2;

	return float4(diffuse + specular + ambient, alpha) * pin.color;
}


#if 0
#include "phong_shader.hlsli"

Texture2D color_map : register(t0);
Texture2D normal_map : register(t1);

SamplerState color_sampler_state : register(s0);


float4 main(VS_OUT pin) : SV_TARGET
{

	float4 diffuse_color = color_map.Sample(color_sampler_state, pin.texcoord);

	float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
	float3 L = normalize(directional_light_direction.xyz);
	float3x3 mat = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
	float3 N = normal_map.Sample(color_sampler_state, pin.texcoord).rgb;
	//ノーマルテクスチャの法線ベクトルを-1.0fから1.0fに変換してワールド座標系へ変換する
	N = normalize(mul(N * 2.0f - 1.0f, mat));

	float3 directional_diffuse = ClacHalfLambert(N, L, directional_light_color.rgb, kd.rgb);

	float3 directional_specular = CalcPhongSpecular(N, L, E, directional_light_color.rgb, ks.rgb);

#if 0
	float4 color = diffuse_color;
	color.rgb *= (ambient + directional_diffuse);
	color.rgb += directional_specular;
#else
	float4 color = float4(ambient, diffuse_color.a);
	color.rgb += diffuse_color.rgb * (directional_diffuse + point_diffuse + spot_diffuse);
	color.rgb += directional_specular + spot_specular + point_specular;

	float3 ambient = ambient_color.rgb * ka.rgb;
	ambient += CalcHemiSphereLight(N, float3(0, 1, 0), sky_color.rgb, ground_color.rgb, hemisphere_weight);

	//フォグの計算
	color = CalcFog(color, fog_color, fog_range.xy, length(pin.world_position.xyz - camera_position.xyz));
	
	return color;
#endif
}
#endif