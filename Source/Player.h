#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Effect/Effect.h"

#include "Character.h"

// プレイヤー
class Player : public Character
{
public:
	Player();
	~Player() override;

	// 更新処理
	void update(float elapsedTime);

	// 描画処理
	void render(ID3D11DeviceContext* dc);

	// デバッグ用の描画
	void drawDebugGui();

	// デバッグプリミティブの描画
	void drawDrawPrimitive();

private:
	// 入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 getMoveVec() const;

	// 移動入力処理
	void inputMove(float elapsedTime);

	// プレイヤーと敵との衝突処理
	void collisionPlayerAndEnemies();

	// ジャンプ入力処理
	void inputJump();

	void inputThrow();

	// 入力による弾発射処理
	void inputLaunchBullet();

protected:
	// 着地したときに呼び出される
	void onLanding() override;

private:
	SkinnedMesh* model = nullptr;
	SkinnedMesh* throw_mdl = nullptr;

	// 移動スピード
	float moveSpeed = 5.0f;

	// 旋回スピード
	float turnSpeed = DirectX::XMConvertToRadians(720);

	// ジャンプ力
	float jumpSpeed = 20.0f;
	// 重力（フレーム単位の値）
	float gravity = -1.0f;
	// 速度
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	int jumpCount = 0;	// ジャンプ回数
	int jumpLimit = 2;	// ジャンプ制限（最大ジャンプ数）
	// ヒットエフェクト
	Effect* hitEffect = nullptr;
};