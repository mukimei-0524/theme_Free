#pragma once

#include <d3d11.h>
#include "Collision.h"

// �X�e�[�W
class Stage
{
public:
	Stage() {}
	virtual ~Stage() {}

	// �X�V����
	virtual void update(float elapsedTime) = 0;

	// �`�揈��
	virtual void render(ID3D11DeviceContext* dc) = 0;

	// ���C�L���X�g
	virtual bool rayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;
};