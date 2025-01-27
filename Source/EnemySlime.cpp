#include "EnemySlime.h"
#include "DeviceManager.h"

EnemySlime::EnemySlime()
{
	model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Slime\\Slime.cereal");

	// モデルが大きいのでスケール調整
	const float scale_fcator = 0.01f;
	scale = { scale_fcator, scale_fcator, scale_fcator };

	radius = 0.5f;	// 幅
	height = 1.0f;	// 高さ
}

EnemySlime::~EnemySlime()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

// 更新処理
void EnemySlime::update(float elapsedTime)
{
	// 速度処理更新
	updateVelocity(elapsedTime);

	// 無敵時間の更新
	updateInvincibleTimer(elapsedTime);

	// ワールド行列の更新
	updateTransform();
}

// 描画処理
void EnemySlime::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

// 死亡したときに呼ばれる
void EnemySlime::onDead()
{
	destroy();
}