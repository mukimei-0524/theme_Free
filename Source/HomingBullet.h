#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Bullet.h"

// �ǔ��e
class HomingBullet : public Bullet
{
public:
	HomingBullet(BulletManager* manager);
	~HomingBullet() override;

	// �X�V����
	void update(float elapsedTime) override;

	// �`�揈��
	void render(ID3D11DeviceContext* dc) override;

	// �ǔ��̃^�[�Q�b�g�̐ݒ�
	void lockonTarget(DirectX::XMFLOAT3 target)
	{ 
		this->target = target;

		// �^�[�Q�b�g�ݒ肵����Ɏp���s�񂪕ύX�����̂�
		// ���̃^�C�~���O�Ŏp���s��̍X�V���K�v
		updateTransform();
	}

private:
	SkinnedMesh* model;
	DirectX::XMFLOAT3 target = { 0,0,0 };	// �ǔ��^�[�Q�b�g
	float speed = 5.0f; // �ړ����x
	float turnSpeed = DirectX::XMConvertToRadians(2.0f); // �ő��]���x
	float lifeTimer = 5.0f; // ����(�R�b)
};