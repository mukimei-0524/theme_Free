#define USE_IMGUI

#include "GameScene.h"
#include "misc.h"

#include "DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include "Camera.h"

#include "EnemyManager.h"
#include "EnemySlime.h"
#include "Effect/EffectManager.h"
#include "StageManager.h"
#include "Input/InputManager.h"
#include "StageManager.h"
#include "StageMain.h"

// 初期化処理
void GameScene::initialize()
{
	// シーン定数バッファの作成
	createBuffer<GameScene::SceneConstants>(DeviceManager::instance()->getDevice(), buffer.GetAddressOf());

	// ステージの作成
	StageManager* stageManager = StageManager::instance();
	StageMain* stageMain = new StageMain();
	stageManager->regist(stageMain);

	// プレイヤーの作成
	player = new Player();

	// カメラ初期設定
	DeviceManager* deviceMgr = DeviceManager::instance();
	Camera* camera = Camera::instance();
	camera->setLookAt(
		DirectX::XMFLOAT3(0, 10, -10),	// カメラの視点（位置）
		DirectX::XMFLOAT3(0, 0, 0),		// カメラの注視点（ターゲット）
		DirectX::XMFLOAT3(0, 1, 0)		// カメラの上方向
	);
	camera->setPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 視野角
		deviceMgr->getScreenWidth() / deviceMgr->getScreenHeight(),	// 画面縦横比率
		0.1f,		// カメラが映し出す最近距離
		1000.0f		// カメラが映し出す最遠距離
	);

	// カメラ操作の初期化
	cameraCtrl = new CameraController();

	EnemyManager* eneMgr = EnemyManager::instance();

	for (int i = 0; i < 2; ++i)
	{
		// 敵の作成と初期化
		EnemySlime* slime = new EnemySlime();
		slime->setPosition({ i * 2.0f,0,5 });

		// 敵管理クラスに取り付け
		eneMgr->regist(slime);
	}

	// ゲージ用スプライト（nullptr を設定するとダミーのテクスチャが生成されるように実装しています）
	gauge = new Sprite(deviceMgr->getDevice(), nullptr);
}

// 終了処理
void GameScene::finalize()
{
	// ゲージ用スプライトを破棄
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	// 敵の破棄
	EnemyManager::instance()->clear();

	// カメラ操作の破棄
	if (cameraCtrl != nullptr)
	{
		delete cameraCtrl;
		cameraCtrl = nullptr;
	}

	// プレイヤーの破棄
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// ステージの破棄
	StageManager::instance()->clear();
}

// 更新処理
void GameScene::update(float elapsedTime)
{
	// カメラ操作の更新
	DirectX::XMFLOAT3 target = *(player->getPosition());
	target.y += 0.5f;	// プレイヤーの腰のあたりに注視点を設定
	cameraCtrl->setTarget(target);
	cameraCtrl->update(elapsedTime);

	// ステージ更新処理
	StageManager::instance()->update(elapsedTime);

	// プレイヤー更新処理
	player->update(elapsedTime);

	// タッチによる敵の配置
	enemyPlacementByTouch(DeviceManager::instance()->getDeviceContext());

	// 敵更新処理
	EnemyManager::instance()->update(elapsedTime);

	// エフェクト更新処理
	EffectManager::instance()->update(elapsedTime);

#ifdef USE_IMGUI	
	ImGui::Begin("ImGUI");

	//ImGui::SliderFloat("speqler", &scene_constants, 0.0f, 1.0f, 0.0f);

	ImGui::End();
#endif

}

// 描画処理
void GameScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	// 3D モデルの描画に必要な情報
	SceneConstants sc;

	{
		// ビュー行列
		DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(camera->getView());
		// プロジェクション行列
		DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(camera->getProjection());

		DirectX::XMStoreFloat4x4(&sc.viewProjection, View * Projection);

		// ライト方向（下方向）
		sc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	//TODO:IMGUIで扱えるようにする
		sc.cameraPosition.x = camera->getEye()->x;
		sc.cameraPosition.y = camera->getEye()->y;
		sc.cameraPosition.z = camera->getEye()->z;
		sc.cameraPosition.w = 1;
	}

	// 3D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc){
		// サンプラーステートの設定（アニソトロピック）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオン、深度書き込みオン）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 3D 描画
	{
		// 3D 描画に利用する定数バッファの更新と設定
		dc->UpdateSubresource(buffer.Get(), 0, 0, &sc, 0, 0);
		dc->VSSetConstantBuffers(1, 1, buffer.GetAddressOf());
		dc->PSSetConstantBuffers(1, 1, buffer.GetAddressOf());

		// ステージの描画
		StageManager::instance()->render(dc);

		// プレイヤーの描画
		player->render(dc);

		// 敵描画
		EnemyManager::instance()->render(dc);
	}

	// 3D エフェクト描画
	{
		EffectManager::instance()->render(view, proj);
	}

	// 3Dデバッグ描画
	{
		// プレイヤー衝突判定用デバッグ描画
		player->drawDrawPrimitive();

		// 敵衝突判定用デバッグ描画
		EnemyManager::instance()->drawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics->getLineRenderer()->render(dc, *view, *proj);

		// デバッグレンダラ描画実行
		graphics->getDebugRenderer()->render(dc, *view, *proj);
	}

	// 2D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（リニア）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 2D 描画
	{
		renderEnemyGauge(dc, view, proj);
	}

	// 2DデバッグGUI描画
	{
		// プレイヤーのデバッグの描画
		player->drawDebugGui();
	}
}

// タッチによる敵の配置
void GameScene::enemyPlacementByTouch(ID3D11DeviceContext* dc)
{
	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(proj);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	// 敵配置処理
	Mouse* mouse = InputManager::instance()->getMouse();
	if (mouse->getButtonDown() & Mouse::BTN_LEFT)
	{
		// マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse->getPositionX());
		screenPosition.y = static_cast<float>(mouse->getPositionY());

		DirectX::XMVECTOR screenPositionVec, worldPositionVec;

		// レイの始点を計算
		screenPosition.z = 0.0f;	// カメラに一番近い位置
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);
		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		// レイの終点を計算
		screenPosition.z = 1.0f;	// カメラに一番遠い位置
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		// レイキャスト
		HitResult hit;
		if (StageManager::instance()->rayCast(rayStart, rayEnd, hit))
		{
			// 敵を配置
			EnemySlime* slime = new EnemySlime();
			slime->setPosition(hit.position);
			EnemyManager::instance()->regist(slime);
		}
	}

}

// 敵ライフゲージの描画
void GameScene::renderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4* view,
	const DirectX::XMFLOAT4X4* projection)
{
	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(projection);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	// 全ての敵の頭上にライフゲージを表示
	EnemyManager* enemyMgr = EnemyManager::instance();
	int enemyCount = enemyMgr->getEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyMgr->getEnemy(i);
		// エネミーの頭上のワールド座標
		DirectX::XMFLOAT3 worldPosition = *enemy->getPosition();
		worldPosition.y += enemy->getHeight();

		DirectX::XMVECTOR worldPositionVec = DirectX::XMLoadFloat3(&worldPosition);

		// ワールド座標からスクリーン座標へ変換
		DirectX::XMVECTOR screenPositionVec = DirectX::XMVector3Project(
			worldPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);

		// スクリーン座標
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, screenPositionVec);

		// ゲージの長さ
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		float healthRate = enemy->getHealth() / static_cast<float>(enemy->getMaxHealth());

		// ゲージ描画
		gauge->render(
			dc,
			screenPosition.x - gaugeWidth * 0.5f,
			screenPosition.y - gaugeHeight,
			gaugeWidth * healthRate,
			gaugeHeight,
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f,
			0, 0,
			static_cast<float>(gauge->getTextureWidth()),
			static_cast<float>(gauge->getTextureHeight())
		);
	}
}