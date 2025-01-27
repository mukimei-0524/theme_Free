#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>

#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

// ゲームシーン
class GameScene : public Scene
{
public:
	GameScene() {}
	~GameScene() override {}

	// 初期化処理
	void initialize() override;

	// 終了処理
	void finalize() override;

	// 更新処理
	void update(float elapsedTime) override;

	// 描画処理
	void render() override;

private:
	// タッチによる敵の出撃
	void enemyPlacementByTouch(ID3D11DeviceContext* dc);

	// 敵ライフゲージ描画
	void renderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4* view,
		const DirectX::XMFLOAT4X4* projection);

private:
	// シーン定数
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
		DirectX::XMFLOAT4		cameraPosition;
	};
	// シーン定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

private:
	Player* player = nullptr;
	CameraController* cameraCtrl = nullptr;
	Sprite* gauge = nullptr;
};