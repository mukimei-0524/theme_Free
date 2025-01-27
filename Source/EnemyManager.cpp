#include "EnemyManager.h"
#include "Collision.h"

// �X�V����
void EnemyManager::update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->update(elapsedTime);
	}

	// �j������
	for (Enemy* enemy : removes)
	{
		auto itr = std::find(enemies.begin(), enemies.end(), enemy);
		if (itr != enemies.end())
		{
			// �j�����ׂ� enemy �� enemies ���ɑ��݂���ꍇ�A�폜
			delete enemy;
			enemies.erase(itr);
		}
	}
	// �j�����X�g���N���A
	removes.clear();

	// �G���m�̏Փˏ���
	collisoinEnemiesAndEnemies();
}

// �`�揈��
void EnemyManager::render(ID3D11DeviceContext* dc)
{
	for (Enemy* enemy : enemies)
	{
		enemy->render(dc);
	}
}

// �G��o�^
void EnemyManager::regist(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

// �G�̑S�폜
void EnemyManager::clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyManager::drawDebugPrimitive()
{
	for (Enemy* enemy : enemies)
	{
		enemy->drawDebugPrimitive();
	}
}

// �G���m�̏Փˏ���
void EnemyManager::collisoinEnemiesAndEnemies() 
{
	int enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* eneA = enemies.at(i);
		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* eneB = enemies.at(j);

			// �Փ˕���
			DirectX::XMFLOAT3 outVec;
			// �Փˏ���
			if (Collision::intersectCylinderAndCylinder(
				*eneA->getPosition(),
				eneA->getRadius(),
				eneA->getHeight(),
				*eneB->getPosition(),
				eneB->getRadius(),
				eneA->getHeight(),
				outVec
			))
			{
				// ���a�̍��v
				float range = eneA->getRadius() + eneB->getRadius();

				// �Փ˕����̃x�N�g���𔼌a�̍��v�̒����ɃX�P�[�����O
				DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

				// ���̃x�N�g�����Փ˂��鑤�̈ʒu�ɑ���
				vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(eneA->getPosition()), vec);

				// ���̈ʒu�x�N�g�����Փ˂���鑤�ɐݒ肷��
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, vec);
				eneB->setPosition(pos);
			}
		}
	}
}

// ��ԋ߂��̓G���擾
Enemy* EnemyManager::searchEnemy(DirectX::XMFLOAT3* position)
{
	Enemy* ene = nullptr;

	// ��ԋ߂��̓G���^�[�Q�b�g�ɐݒ�
	float dist = FLT_MAX;
	int enemyCount = static_cast<int>(enemies.size());
	for (int i = 0; i < enemyCount; ++i)
	{
		// �G�Ƃ̋����𔻒�
		Enemy* enemy = enemies.at(i);
		DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(position);
		DirectX::XMVECTOR eneVec = DirectX::XMLoadFloat3(enemy->getPosition());
		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(eneVec, posVec);
		DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
		float lengthSq;
		DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
		if (lengthSq < dist)
		{
			dist = lengthSq;
			ene = enemy;
		}
	}

	return ene;
}

// �G�̍폜
void EnemyManager::remove(Enemy* enemy)
{
	auto itr = std::find(removes.begin(), removes.end(), enemy);

	// �d�����Ă�����ǉ����Ȃ�
	if (itr != removes.end())
		return;

	// �j�����X�g�ɒǉ�
	removes.push_back(enemy);
}