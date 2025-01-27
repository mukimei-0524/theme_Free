#include "Enemy.h"
#include "Graphics/GraphicsManager.h"
#include "EnemyManager.h"

// デバッグプリミティブの描画
void Enemy::drawDebugPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	// 衝突判定用のデバッグ用円柱を描画
	debugRenderer->drawCylinder(position, radius, height, { 0, 0, 0, 1 });
}

// 破棄
void Enemy::destroy()
{
	EnemyManager::instance()->remove(this);
}