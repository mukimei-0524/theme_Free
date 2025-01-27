#include "BulletManager.h"

// �R���X�g���N�^
BulletManager::BulletManager()
{

}

// �f�X�g���N�^
BulletManager::~BulletManager()
{
	clear();
}

// �X�V����
void BulletManager::update(float elapsedTime)
{
	for (Bullet* bullet : bullets)
	{
		bullet->update(elapsedTime);
	}

	// �j������
	for (Bullet* bullet : removes)
	{
		auto itr = std::find(bullets.begin(), bullets.end(), bullet);
		if (itr != bullets.end())
		{
			// �j�����ׂ� bullet �� bullets ���ɑ��݂���ꍇ�A�폜
			delete bullet;
			bullets.erase(itr);
		}
	}
	// �j�����X�g���N���A
	removes.clear();
}

// �`�揈��
void BulletManager::render(ID3D11DeviceContext* dc)
{
	for (Bullet* bullet : bullets)
	{
		bullet->render(dc);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void BulletManager::drawDebugPrimitive()
{
	for (Bullet* bullet : bullets)
	{
		bullet->drawDebugPrimitive();
	}
}

// �e�o�^
void BulletManager::regist(Bullet* bullet)
{
	bullets.emplace_back(bullet);
}

// �e�S�폜
void BulletManager::clear()
{
	for(Bullet* bullet : bullets)
	{
		delete bullet;
	}
	bullets.clear();
}

// �e�폜
void BulletManager::remove(Bullet* bullet)
{
	auto itr = std::find(removes.begin(), removes.end(), bullet);

	// �d�����Ă�����ǉ����Ȃ�
	if (itr != removes.end())
		return;

	// �j�����X�g�ɒǉ�
	removes.push_back(bullet);
}