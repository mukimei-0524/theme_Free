/// <summary>
/// ランバート拡散反射
/// </summary>
/// <param name="N">法線</param>
/// <param name="L">入射ベクトル</param>
/// <param name="C">入射光(色・強さ)</param>
/// <param name="K">反射率</param>
/// <returns></returns>
float3 CalcLambert(float3 N, float3 L, float3 C, float3 K)
{
	float power = saturate(dot(N, -L));
	return C * power * K;
}

/// <summary>
/// フォンの鏡面反射
/// </summary>
/// <param name="N"></param>
/// <param name="L"></param>
/// <param name="E"></param>
/// <param name="C"></param>
/// <param name="K"></param>
/// <returns></returns>
float3 CalcPhongSpecular(float3 N, float3 L, float3 E, float3 C, float3 K)
{
	float3 R = reflect(L, N);
	float power = max(dot(-E, R), 0);
	power = pow(power, 128);
	return C * power * K;
}

/// <summary>
/// ハーフランバート拡散反射
/// </summary>
/// <param name="N">ハーフランバート拡散反射</param>
/// <param name="L">入射ベクトル</param>
/// <param name="C">入射光</param>
/// <param name="K">反射率</param>
/// <returns></returns>
float3 ClacHalfLambert(float3 N, float3 L, float3 C, float3 K)
{
	float D = saturate(dot(N, -L) * 0.5f + 0.5f);
	return C * D * K;
}

/// <summary>
/// リムライト
/// </summary>
/// <param name="N">法線</param>
/// <param name="E">視点方向ベクトル</param>
/// <param name="L">入射ベクトル</param>
/// <param name="C">ライト色</param>
/// <param name="RimPower">リムライトの強さ</param>
/// <returns></returns>
float3 CalcRimLight(float3 N, float3 E, float3 L, float3 C, float RimPower = 3.0f)
{
	float rim = 1.0f - saturate(dot(N, -E));
	return  C * pow(rim, RimPower) * saturate(dot(L, -E));
}

/// <summary>
/// ランプシェーディング
/// </summary>
/// <param name="tex">ランプシェーディング用テクスチャ</param>
/// <param name="samp">ランプシェーディング用サンプラステート</param>
/// <param name="N">法線</param>
/// <param name="L">入射ベクトル</param>
/// <param name="C">入射光(色・強さ)</param>
/// <param name="K">反射率</param>
/// <returns></returns>
float3 CalcRampShading(Texture2D tex, SamplerState samp, float3 N, float3 L, float3 C, float3 K)
{
	float D = saturate(dot(N, -L) * 0.5f + 0.5f);
	float Ramp = tex.Sample(samp, float2(D, 0.5f)).r;
	return  C * Ramp * K.rgb;
}


/// <summary>
/// 球体環境マッピング(スフィア環境マッピング)
/// </summary>
/// <param name="tex">スフィアマッピング用テクスチャ</param>
/// <param name="samp">スフィアマッピング用サンプラーステート</param>
/// <param name="color">現在のピクセル色</param>
/// <param name="N">法線</param>
/// <param name="E">入射光(色・強さ)</param>
/// <param name="value">適応率</param>
/// <returns></returns>

float3 CalcSphereEnvironment(Texture2D tex, SamplerState samp, in float3 color, float3 N, float3 E, float value)
{
	float3 R = reflect(E, N);
	float2 texcoord = R.xy * 0.5f + 0.5f;

	//例＞colorが車の色、texが映りこむ背景の色、valueが0.5の場合
	//２つの色が混ざった色が最終的に表示される色になる
	//鏡の場合はvalueが1.0となり、texのみが最終的に表示させる色になる
	return	lerp(color.rgb, tex.Sample(samp, texcoord).rgb, value);
}


/// <summary>
/// 半球ライティング
/// </summary>
/// <param name="normal">法線(正規化済み)</param>
/// <param name="up">上方向（片方)</param>
/// <param name="sky_color"> sky_color:空(上)色</param>
/// <param name="ground_color"> ground_color:地面(下)色</param>
/// <param name="hemisphere_weight">hemisphere_weight:重み</param>
/// <returns></returns>
float3 CalcHemiSphereLight(float3 normal, float3 up, float3 sky_color, float3 ground_color, float4 hemisphere_weight)
{
	float factor = dot(normal, up) * 0.5f + 0.5f;
	return lerp(ground_color, sky_color, factor) * hemisphere_weight.x;
}

//フォグ
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
	float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
	return lerp(color, fog_color, fogAlpha);
}