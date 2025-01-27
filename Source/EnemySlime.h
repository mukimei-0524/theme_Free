#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Enemy.h"

// スライム
class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime() override;

	// 更新処理
	void update(float elapsedTime) override;

	// 描画処理
	void render(ID3D11DeviceContext* dc) override;

	// 死亡したときに呼ばれる
	void onDead() override;

private:
	SkinnedMesh* model = nullptr;
};