#include "StraightBullet.h"
#include "DeviceManager.h"

// �R���X�g���N�^
StraightBullet::StraightBullet(BulletManager* manager)
	: Bullet(manager)
{
	model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Sword\\Sword.cereal");

	const float scale_fcator = 3.0f;
	scale = { scale_fcator, scale_fcator, scale_fcator };
}

// �f�X�g���N�^
StraightBullet::~StraightBullet()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

// �X�V����
void StraightBullet::update(float elapsedTime)
{
	// �o�ߎ��Ԃ������Ă���
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f)
	{
		// ���Ԃ� 0 �ȉ��ɂȂ����玩����j��
		destroy();
	}

	// �ړ�
	float speed = this->speed * elapsedTime;

	// �ʒu += ���� * ����
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;

	// ���[���h�s��̍X�V
	updateTransform();
}

// �`�揈��
void StraightBullet::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}