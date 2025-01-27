#pragma once

#include "Bullet.h"
#include <vector>

// �e�Ǘ�
class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	// �X�V����
	void update(float elpasedTime);

	// �`�揈��
	void render(ID3D11DeviceContext* dc);

	// �f�o�b�O�v���~�e�B�u�`��
	void drawDebugPrimitive();

	// �e�o�^
	void regist(Bullet* bullet);

	// �e�S�폜
	void clear();

	// �e���擾
	int getBulletCount() const { return static_cast<int>(bullets.size()); }

	// �e�擾
	Bullet* getBullet(int index) { return bullets.at(index); }

	// �e�폜
	void remove(Bullet* bullet);

private:
	std::vector<Bullet*> bullets;
	std::vector<Bullet*> removes;
};