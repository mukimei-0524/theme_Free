#include "EnemySlime.h"
#include "DeviceManager.h"

EnemySlime::EnemySlime()
{
	model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Slime\\Slime.cereal");

	// ���f�����傫���̂ŃX�P�[������
	const float scale_fcator = 0.01f;
	scale = { scale_fcator, scale_fcator, scale_fcator };

	radius = 0.5f;	// ��
	height = 1.0f;	// ����
}

EnemySlime::~EnemySlime()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

// �X�V����
void EnemySlime::update(float elapsedTime)
{
	// ���x�����X�V
	updateVelocity(elapsedTime);

	// ���G���Ԃ̍X�V
	updateInvincibleTimer(elapsedTime);

	// ���[���h�s��̍X�V
	updateTransform();
}

// �`�揈��
void EnemySlime::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

// ���S�����Ƃ��ɌĂ΂��
void EnemySlime::onDead()
{
	destroy();
}