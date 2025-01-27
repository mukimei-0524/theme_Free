#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Bullet.h"

// 追尾弾
class HomingBullet : public Bullet
{
public:
	HomingBullet(BulletManager* manager);
	~HomingBullet() override;

	// 更新処理
	void update(float elapsedTime) override;

	// 描画処理
	void render(ID3D11DeviceContext* dc) override;

	// 追尾のターゲットの設定
	void lockonTarget(DirectX::XMFLOAT3 target)
	{ 
		this->target = target;

		// ターゲット設定した後に姿勢行列が変更されるので
		// このタイミングで姿勢行列の更新が必要
		updateTransform();
	}

private:
	SkinnedMesh* model;
	DirectX::XMFLOAT3 target = { 0,0,0 };	// 追尾ターゲット
	float speed = 5.0f; // 移動速度
	float turnSpeed = DirectX::XMConvertToRadians(2.0f); // 最大回転速度
	float lifeTimer = 5.0f; // 寿命(３秒)
};