/// <summary>
/// �����o�[�g�g�U����
/// </summary>
/// <param name="N">�@��</param>
/// <param name="L">���˃x�N�g��</param>
/// <param name="C">���ˌ�(�F�E����)</param>
/// <param name="K">���˗�</param>
/// <returns></returns>
float3 CalcLambert(float3 N, float3 L, float3 C, float3 K)
{
	float power = saturate(dot(N, -L));
	return C * power * K;
}

/// <summary>
/// �t�H���̋��ʔ���
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
/// �n�[�t�����o�[�g�g�U����
/// </summary>
/// <param name="N">�n�[�t�����o�[�g�g�U����</param>
/// <param name="L">���˃x�N�g��</param>
/// <param name="C">���ˌ�</param>
/// <param name="K">���˗�</param>
/// <returns></returns>
float3 ClacHalfLambert(float3 N, float3 L, float3 C, float3 K)
{
	float D = saturate(dot(N, -L) * 0.5f + 0.5f);
	return C * D * K;
}

/// <summary>
/// �������C�g
/// </summary>
/// <param name="N">�@��</param>
/// <param name="E">���_�����x�N�g��</param>
/// <param name="L">���˃x�N�g��</param>
/// <param name="C">���C�g�F</param>
/// <param name="RimPower">�������C�g�̋���</param>
/// <returns></returns>
float3 CalcRimLight(float3 N, float3 E, float3 L, float3 C, float RimPower = 3.0f)
{
	float rim = 1.0f - saturate(dot(N, -E));
	return  C * pow(rim, RimPower) * saturate(dot(L, -E));
}

/// <summary>
/// �����v�V�F�[�f�B���O
/// </summary>
/// <param name="tex">�����v�V�F�[�f�B���O�p�e�N�X�`��</param>
/// <param name="samp">�����v�V�F�[�f�B���O�p�T���v���X�e�[�g</param>
/// <param name="N">�@��</param>
/// <param name="L">���˃x�N�g��</param>
/// <param name="C">���ˌ�(�F�E����)</param>
/// <param name="K">���˗�</param>
/// <returns></returns>
float3 CalcRampShading(Texture2D tex, SamplerState samp, float3 N, float3 L, float3 C, float3 K)
{
	float D = saturate(dot(N, -L) * 0.5f + 0.5f);
	float Ramp = tex.Sample(samp, float2(D, 0.5f)).r;
	return  C * Ramp * K.rgb;
}


/// <summary>
/// ���̊��}�b�s���O(�X�t�B�A���}�b�s���O)
/// </summary>
/// <param name="tex">�X�t�B�A�}�b�s���O�p�e�N�X�`��</param>
/// <param name="samp">�X�t�B�A�}�b�s���O�p�T���v���[�X�e�[�g</param>
/// <param name="color">���݂̃s�N�Z���F</param>
/// <param name="N">�@��</param>
/// <param name="E">���ˌ�(�F�E����)</param>
/// <param name="value">�K����</param>
/// <returns></returns>

float3 CalcSphereEnvironment(Texture2D tex, SamplerState samp, in float3 color, float3 N, float3 E, float value)
{
	float3 R = reflect(E, N);
	float2 texcoord = R.xy * 0.5f + 0.5f;

	//�၄color���Ԃ̐F�Atex���f�肱�ޔw�i�̐F�Avalue��0.5�̏ꍇ
	//�Q�̐F�����������F���ŏI�I�ɕ\�������F�ɂȂ�
	//���̏ꍇ��value��1.0�ƂȂ�Atex�݂̂��ŏI�I�ɕ\��������F�ɂȂ�
	return	lerp(color.rgb, tex.Sample(samp, texcoord).rgb, value);
}


/// <summary>
/// �������C�e�B���O
/// </summary>
/// <param name="normal">�@��(���K���ς�)</param>
/// <param name="up">������i�Е�)</param>
/// <param name="sky_color"> sky_color:��(��)�F</param>
/// <param name="ground_color"> ground_color:�n��(��)�F</param>
/// <param name="hemisphere_weight">hemisphere_weight:�d��</param>
/// <returns></returns>
float3 CalcHemiSphereLight(float3 normal, float3 up, float3 sky_color, float3 ground_color, float4 hemisphere_weight)
{
	float factor = dot(normal, up) * 0.5f + 0.5f;
	return lerp(ground_color, sky_color, factor) * hemisphere_weight.x;
}

//�t�H�O
float4 CalcFog(in float4 color, float4 fog_color, float2 fog_range, float eye_length)
{
	float fogAlpha = saturate((eye_length - fog_range.x) / (fog_range.y - fog_range.x));
	return lerp(color, fog_color, fogAlpha);
}