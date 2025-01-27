#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Collision.h"
#include "Stage.h"

// �X�e�[�W
class StageMain : public Stage
{
public:
	StageMain();
	~StageMain() override;

	// �X�V����
	void update(float elapsedTime) override;

	// �`�揈��
	void render(ID3D11DeviceContext* dc) override;

	// ���C�L���X�g
	bool rayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	SkinnedMesh* model = nullptr;
};