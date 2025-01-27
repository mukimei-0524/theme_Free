//ÉtÉHÉO
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
	float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
	return lerp(color, fog_color, fogAlpha);
}