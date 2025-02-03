#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Effect/Effect.h"

#include "Character.h"

// �v���C���[
class Player : public Character
{
public:
	Player();
	~Player() override;

	// �X�V����
	void update(float elapsedTime);

	// �`�揈��
	void render(ID3D11DeviceContext* dc);

	// �f�o�b�O�p�̕`��
	void drawDebugGui();

	// �f�o�b�O�v���~�e�B�u�̕`��
	void drawDrawPrimitive();

private:
	// ���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 getMoveVec() const;

	// �ړ����͏���
	void inputMove(float elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	void collisionPlayerAndEnemies();

	// �W�����v���͏���
	void inputJump();

	void inputThrow();

	// ���͂ɂ��e���ˏ���
	void inputLaunchBullet();

protected:
	// ���n�����Ƃ��ɌĂяo�����
	void onLanding() override;

private:
	SkinnedMesh* model = nullptr;
	SkinnedMesh* throw_mdl = nullptr;

	// �ړ��X�s�[�h
	float moveSpeed = 5.0f;

	// ����X�s�[�h
	float turnSpeed = DirectX::XMConvertToRadians(720);

	// �W�����v��
	float jumpSpeed = 20.0f;
	// �d�́i�t���[���P�ʂ̒l�j
	float gravity = -1.0f;
	// ���x
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	int jumpCount = 0;	// �W�����v��
	int jumpLimit = 2;	// �W�����v�����i�ő�W�����v���j
	// �q�b�g�G�t�F�N�g
	Effect* hitEffect = nullptr;
};