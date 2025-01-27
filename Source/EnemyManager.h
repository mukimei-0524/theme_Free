#pragma once

#include <vector>
#include "Enemy.h"

// �G�Ǘ�
class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}

public:
	// singleton
	static EnemyManager* instance()
	{
		static EnemyManager inst;
		return &inst;
	}

	// �X�V����
	void update(float elapsedTime);

	// �`�揈��
	void render(ID3D11DeviceContext* dc);

	// �G�̓o�^
	void regist(Enemy* enemy);

	// �G�̔j��
	void clear();

	// �f�o�b�O�v���~�e�B�u�`��
	void drawDebugPrimitive();

	// �G�̑������擾
	int getEnemyCount() const { return static_cast<int>(enemies.size()); }

	// �G���擾
	Enemy* getEnemy(int index) { return enemies.at(index); }

	// ��ԋ߂��̓G���擾
	Enemy* searchEnemy(DirectX::XMFLOAT3* position);

	// �G�̍폜
	void remove(Enemy* enemy);

private:
	// �G���m�̏Փˏ���
	void collisoinEnemiesAndEnemies();

private:
	std::vector<Enemy*>	enemies;
	std::vector<Enemy*> removes;
};