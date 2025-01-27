#pragma once

#include <DirectXMath.h>

// �L�����N�^�[
class Character
{
public:
	Character() {}
	virtual ~Character() {}

	// �s��X�V����
	void updateTransform();

	// �ʒu�擾
	DirectX::XMFLOAT3* getPosition() { return &position; }

	// �ʒu�ݒ�
	void setPosition(const DirectX::XMFLOAT3& p) { this->position = p; }

	// ��]�擾
	const DirectX::XMFLOAT3* getAngle() const { return &angle; }

	// ��]�ݒ�
	void setAngle(const DirectX::XMFLOAT3& a) { this->angle = a; }

	// �g��k���擾
	const DirectX::XMFLOAT3* getScale() const { return &scale; }

	// �g��k���ݒ�
	void setScale(const DirectX::XMFLOAT3& s) { this->scale = s; }

	// ���a�̎擾
	float getRadius() const { return radius; }

	// ���n�������ǂ����̃t���O�̎擾
	bool isGrounded() const { return groundedFlag; }

	// �����擾
	float getHeight() const { return height; }

	// ���C�t���擾
	int getHealth() const { return health; }

	// �ő僉�C�t���擾
	int getMaxHealth() const { return maxHealth; }

	/// <summary>
	/// �_���[�W����
	/// </summary>
	/// <param name="damage">�_���[�W��</param>
	/// <param name="timer">���G����</param>
	/// <returns>true...HP�ɕω���������</returns>
	bool applyDamage(int damage, float timer = 0.5f);

	// �Ռ���^����
	void addImpulse(const DirectX::XMFLOAT3& impulse);

protected:
	// �ړ�����������
	void move(float vx, float vz, float speed);

	// ���񏈗�
	void turn(float elapsedTime, float vx, float vz, float speed);

	// �W�����v����
	void jump(float speed);

	// ���x�����X�V
	void updateVelocity(float elpasedTime);

	// ���n�����Ƃ��ɌĂяo�����
	virtual void onLanding() {}

	// �_���[�W���󂯂��Ƃ��ɌĂ΂��
	virtual void onDamaged() {}

	// ���S�����Ƃ��ɌĂ΂��
	virtual void onDead() {}

	// ���G���Ԃ̍X�V
	void updateInvincibleTimer(float elapsedTime);

private:
	// �������x�X�V����
	void updateVerticalVelocity(float elpasedTime);

	// �����ړ��X�V����
	void updateVerticalMove(float elapsedTime);

	// �������x�X�V����
	void updateHorizontalVelocity(float elapsedTime);

	// �����ړ��X�V����
	void updateHorizontalMove(float elapsedTime);

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };	// �ʒu
	DirectX::XMFLOAT3 angle = { 0,0,0 };	// ��]
	DirectX::XMFLOAT3 scale = { 1,1,1 };	// �g��k��
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float radius = 0.5f; // ���a
	float gravity = -1.0f;	// �d�́i�t���[���P�ʂ̒l�j
	DirectX::XMFLOAT3 velocity = { 0,0,0 };	// ���x
	bool groundedFlag = false;	// true...���n����
	float height = 2.0f;	// ����
	int health = 5;	// ���C�t
	int maxHealth = 5;	// �ő僉�C�t
	float invincibleTimer = 0.5f;	// ���G���ԁi0.5�b�j
	float friction = 0.5f; // ���C
	float acceleration = 1.0f;	// �����x
	float maxMoveSpeed = 5.0f;	// �ő�ړ����x
	DirectX::XMFLOAT3 direction = { 0,0,0 };	// �ړ�����
	float airControl = 0.3f;	// �󒆐���p�W��
	float slopeRate = 1.0f; // �X�Η�
};