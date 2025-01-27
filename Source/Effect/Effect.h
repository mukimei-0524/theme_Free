#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <set>

// �G�t�F�N�g
class Effect
{
public:
	Effect(const char* filename);
	~Effect() {}

	// �Đ�
	Effekseer::Handle play(const DirectX::XMFLOAT3* position, float scale = 1.0f);

	// ��~
	void stop(Effekseer::Handle handle);

	// �ʒu�ݒ�
	void setPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3* position);

	// �g��k���ݒ�
	void setScale(Effekseer::Handle handle, const DirectX::XMFLOAT3* scale);

private:
	Effekseer::EffectRef effekseerEffect;
};