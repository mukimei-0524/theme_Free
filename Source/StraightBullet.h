#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Bullet.h"

// ���i�e
class StraightBullet : public Bullet
{
public:
	StraightBullet(BulletManager* manager);
	~StraightBullet() override;

	// �X�V����
	void update(float elapsedTime) override;

	// �`�揈��
	void render(ID3D11DeviceContext* dc) override;

private:
	SkinnedMesh* model = nullptr;
	float speed = 10.0f;
	float lifeTimer = 3.0f;	// ����(�R�b)
};
