#include "Bullet.h"
#include "BulletManager.h"
#include "Graphics/GraphicsManager.h"

// �R���X�g���N�^
Bullet::Bullet(BulletManager* manager)
	: manager(manager)
{
	manager->regist(this);
}

// �f�o�b�O�v���~�e�B�u�`��
void Bullet::drawDebugPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	// �Փ˔���p�̃f�o�b�O�p����`��
	debugRenderer->drawSphere(position, radius, { 0, 0, 0, 1 });
}

void Bullet::updateTransform()
{
	DirectX::XMVECTOR frontVec, upVec, rightVec;

	// ���݂̕����𐳋K�����O�����x�N�g�����v�Z
	frontVec = DirectX::XMLoadFloat3(&direction);
	frontVec = DirectX::XMVector3Normalize(frontVec);

	// ���̏�����x�N�g�����v�Z
	upVec = DirectX::XMVectorSet(0.001f, 1, 0, 0);
	upVec = DirectX::XMVector3Normalize(upVec);

	// �O�ς𗘗p���ĉE�����x�N�g�����v�Z
	rightVec = DirectX::XMVector3Cross(upVec, frontVec);
	rightVec = DirectX::XMVector3Normalize(rightVec);

	// �O�ς𗘗p���Đ�����������x�Na�g�����v�Z
	upVec = DirectX::XMVector3Cross(frontVec, rightVec);

	// �v�Z���ʂ� XMFLOAT3 �ɕϊ�
	DirectX::XMFLOAT3 right, up, front;
	DirectX::XMStoreFloat3(&right, rightVec);
	DirectX::XMStoreFloat3(&up, upVec);
	DirectX::XMStoreFloat3(&front, frontVec);

	// �v�Z�������x�N�g������p���s����쐬
	transform._11 = right.x * scale.x;
	transform._12 = right.y * scale.x;
	transform._13 = right.z * scale.x;
	transform._14 = 0.0f;
	
	transform._21 = up.x * scale.y;
	transform._22 = up.y * scale.y;
	transform._23 = up.z * scale.y;
	transform._24 = 0.0;
	
	transform._31 = front.x * scale.z;
	transform._32 = front.y * scale.z;
	transform._33 = front.z * scale.z;
	transform._34 = 0.0f;
	
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

	// ���˕���
	this->direction = front;
}

// �j��
void Bullet::destroy()
{
	manager->remove(this);
}