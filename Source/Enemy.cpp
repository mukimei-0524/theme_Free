#include "Enemy.h"
#include "Graphics/GraphicsManager.h"
#include "EnemyManager.h"

// �f�o�b�O�v���~�e�B�u�̕`��
void Enemy::drawDebugPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	// �Փ˔���p�̃f�o�b�O�p�~����`��
	debugRenderer->drawCylinder(position, radius, height, { 0, 0, 0, 1 });
}

// �j��
void Enemy::destroy()
{
	EnemyManager::instance()->remove(this);
}