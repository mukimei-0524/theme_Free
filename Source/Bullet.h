#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

// �e�Ǘ��̑O���錾
class BulletManager;

// �e
class Bullet
{
public:
	Bullet(BulletManager* manager);
	virtual ~Bullet() {}

	// �X�V����
	virtual void update(float elapsedTime) = 0;

	// �`�揈��
	virtual void render(ID3D11DeviceContext* dc) = 0;

	// �f�o�b�O�v���~�e�B�u�`��
	virtual void drawDebugPrimitive();

	// �ʒu�擾
	const DirectX::XMFLOAT3* getPosition() const { return &position; }

	// �ʒu�̐ݒ�
	void setPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// �����擾
	const DirectX::XMFLOAT3* getDirection() const { return &direction; }

	// �����̐ݒ�
	void setDirection(const DirectX::XMFLOAT3& direction) { this->direction = direction; }
	
	// �g��k���擾
	const DirectX::XMFLOAT3* getScale() const { return &scale; }

	// ���a�擾
	float getRadius() const { return radius; }

	// �j��
	void destroy();

protected:
	// �s��X�V����
	void updateTransform();

protected:
	DirectX::XMFLOAT3	position = { 0,0,0 };	// �ʒu
	DirectX::XMFLOAT3	direction = { 0,0,1 };	// ����
	DirectX::XMFLOAT3	scale = { 1,1,1 };		// �g��k��
	// �p���s��
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	// �e�Ǘ�
	BulletManager* manager;

	float radius = 0.5f;	// ���a
};