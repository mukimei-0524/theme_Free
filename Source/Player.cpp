#include "Graphics/GraphicsManager.h"
#include "Input/InputManager.h"
#include "Graphics/ImGuiRenderer.h" 

#include "DeviceManager.h"
#include "EnemyManager.h"

#include "Collision.h"
#include "Player.h"
#include "Camera.h"

// コンストラクタ
Player::Player()
{
	model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Jummo\\Jummo.cereal");
	throw_mdl = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Sword\\Sword.cereal");

	const float scale_fcator = 0.01f;	// モデルが大きいのでスケール調整
	scale = { scale_fcator, scale_fcator, scale_fcator };

	// ヒットエフェクト読み込み
	hitEffect = new Effect(".\\Resources\\Effect\\01_AndrewFM01\\hit_eff.efk");
}

// デストラクタ
Player::~Player()
{
	if (hitEffect != nullptr)
	{
		delete hitEffect;
		hitEffect = nullptr;
	}

	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}

	if (throw_mdl != nullptr)
	{
		delete throw_mdl;
		throw_mdl = nullptr;
	}
}

// 更新処理
void Player::update(float elapsedTime)
{
	// 移動入力処理
	inputMove(elapsedTime);

	// ジャンプ入力処理
	inputJump();

	inputThrow();

	// 入力による弾発射処理
	inputLaunchBullet();

	// 速度処理更新
	updateVelocity(elapsedTime);

	// プレイヤーと敵との衝突処置
	collisionPlayerAndEnemies();

	// ワールド行列の更新
	updateTransform();
}

// 描画処理
void Player::render(ID3D11DeviceContext* dc)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
	if (gamePad->getButtonDown() & gamePad->BTN_X)
	{
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
	}
}

// デバッグ用の描画
void Player::drawDebugGui()
{
	ImGui::Begin("Player");
	
	// 位置
	ImGui::InputFloat3("Position", &position.x);
	
	// 回転
	DirectX::XMFLOAT3 a{DirectX::XMConvertToDegrees(angle.x), DirectX::XMConvertToDegrees(angle.y) , DirectX::XMConvertToDegrees(angle.z) };
	ImGui::InputFloat3("Angle", &a.x);
	angle = { DirectX::XMConvertToRadians(a.x),DirectX::XMConvertToRadians(a.y), DirectX::XMConvertToRadians(a.z) };

	// 拡大縮小
	ImGui::InputFloat3("Scale", &scale.x);

	ImGui::End();
}

// デバッグプリミティブの描画
void Player::drawDrawPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	// 衝突判定用のデバッグ用円柱を描画
	debugRenderer->drawCylinder(position, radius, height, { 0, 0, 0, 1 });
}

// 入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::getMoveVec() const
{
	// 入力情報を取得
	GamePad* gamePad = InputManager::instance()->getGamePad();
	float ax = gamePad->getAxisLX();
	float ay = gamePad->getAxisLY();

	// カメラ方向を取得
	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT3* cameraFront = camera->getFront();
	const DirectX::XMFLOAT3* cameraRight = camera->getRight();

	// カメラ前方向ベクトルを XZ 単位ベクトルに変換
	float cameraFrontX = cameraFront->x;
	float cameraFrontZ = cameraFront->z;

	// カメラ前方向ベクトルを単位ベクトル化
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		cameraFrontX = cameraFrontX / cameraFrontLength;
		cameraFrontZ = cameraFrontZ / cameraFrontLength;
	}

	// カメラ右方向ベクトルを XZ 単位ベクトルに変換
	float cameraRightX = cameraRight->x;
	float cameraRightZ = cameraRight->z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		cameraRightX = cameraRightX / cameraRightLength;
		cameraRightZ = cameraRightZ / cameraRightLength;
	}

	// 垂直入力値をカメラ前方向に反映し進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = cameraFrontX * ay + cameraRightX * ax;
	vec.z = cameraFrontZ * ay + cameraRightZ * ax;

	// Y 軸方向には移動しない。
	vec.y = 0.0f;

	return vec;
}



// 移動入力処理
void Player::inputMove(float elapsedTime)
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = getMoveVec();

	// 移動処理
	move(moveVec.x, moveVec.z, moveSpeed);

	// 旋回処理
	turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}



// プレイヤーと敵との衝突処理
void Player::collisionPlayerAndEnemies()
{
	EnemyManager* eneMgr = EnemyManager::instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = eneMgr->getEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = eneMgr->getEnemy(i);

		// 衝突方向
		DirectX::XMFLOAT3 outVec;

		// 衝突処理
		if (Collision::intersectCylinderAndCylinder(
			position,
			radius,
			height,
			*(enemy->getPosition()),
			enemy->getRadius(),
			enemy->getHeight(),
			outVec
		))
		{
			OutputDebugStringA("衝突\n");

			// 敵の真上付近にあたったかを判定
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(enemy->getPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			// 上から踏んづけた場合は小ジャンプする
			if (normal.y > 0.8f)
			{
				// 小ジャンプする
				jump(jumpSpeed * 0.5f);
			}
			else
			{
				// 押し出し後の位置設定

				// 半径の合計
				float range = radius + enemy->getRadius();

				// 衝突方向のベクトルを半径の合計の長さにスケーリング
				DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

				// そのベクトルを衝突する側の位置に足す
				vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), vec);

				// その位置ベクトルを衝突される側に設定する
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, vec);
				pos.y = enemy->getPosition()->y;
				enemy->setPosition(pos);
			}
		}
	}
}

// ジャンプ入力処理
void Player::inputJump()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	if (gamePad->getButtonDown() & GamePad::BTN_A)
	{
		// ジャンプ回数制限
		if (jumpCount < jumpLimit)
		{
			// ジャンプ
			jumpCount++;
			jump(jumpSpeed);
		}
	}
}

// 投げる入力処理
void Player::inputThrow()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();

}

// 入力による弾発射処理
void Player::inputLaunchBullet()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();

	// ストレート弾発射
	if (gamePad->getButtonDown() & GamePad::BTN_X)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		// 発射位置（プレイヤーの腰あたり
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
	}

	// ホーミング弾発射
	if (gamePad->getButtonDown() & GamePad::BTN_Y)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		// 発射位置（プレイヤーの腰あたり
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		// ターゲット
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		// 一番近くの敵を取得
		Enemy* enemy = EnemyManager::instance()->searchEnemy(&position);
		if (enemy != nullptr)
		{
			target = *enemy->getPosition();
			target.y += enemy->getHeight() * 0.5f;
		}
	}
}

// 着地時に呼び出される
void Player::onLanding()
{
	jumpCount = 0;
}