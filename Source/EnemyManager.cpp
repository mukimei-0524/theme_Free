#include "EnemyManager.h"
#include "Collision.h"

// 更新処理
void EnemyManager::update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->update(elapsedTime);
	}

	// 破棄処理
	for (Enemy* enemy : removes)
	{
		auto itr = std::find(enemies.begin(), enemies.end(), enemy);
		if (itr != enemies.end())
		{
			// 破棄すべき enemy が enemies 内に存在する場合、削除
			delete enemy;
			enemies.erase(itr);
		}
	}
	// 破棄リストをクリア
	removes.clear();

	// 敵同士の衝突処理
	collisoinEnemiesAndEnemies();
}

// 描画処理
void EnemyManager::render(ID3D11DeviceContext* dc)
{
	for (Enemy* enemy : enemies)
	{
		enemy->render(dc);
	}
}

// 敵を登録
void EnemyManager::regist(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

// 敵の全削除
void EnemyManager::clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

// デバッグプリミティブ描画
void EnemyManager::drawDebugPrimitive()
{
	for (Enemy* enemy : enemies)
	{
		enemy->drawDebugPrimitive();
	}
}

// 敵同士の衝突処理
void EnemyManager::collisoinEnemiesAndEnemies() 
{
	int enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* eneA = enemies.at(i);
		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* eneB = enemies.at(j);

			// 衝突方向
			DirectX::XMFLOAT3 outVec;
			// 衝突処理
			if (Collision::intersectCylinderAndCylinder(
				*eneA->getPosition(),
				eneA->getRadius(),
				eneA->getHeight(),
				*eneB->getPosition(),
				eneB->getRadius(),
				eneA->getHeight(),
				outVec
			))
			{
				// 半径の合計
				float range = eneA->getRadius() + eneB->getRadius();

				// 衝突方向のベクトルを半径の合計の長さにスケーリング
				DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

				// そのベクトルを衝突する側の位置に足す
				vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(eneA->getPosition()), vec);

				// その位置ベクトルを衝突される側に設定する
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, vec);
				eneB->setPosition(pos);
			}
		}
	}
}

// 一番近くの敵を取得
Enemy* EnemyManager::searchEnemy(DirectX::XMFLOAT3* position)
{
	Enemy* ene = nullptr;

	// 一番近くの敵をターゲットに設定
	float dist = FLT_MAX;
	int enemyCount = static_cast<int>(enemies.size());
	for (int i = 0; i < enemyCount; ++i)
	{
		// 敵との距離を判定
		Enemy* enemy = enemies.at(i);
		DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(position);
		DirectX::XMVECTOR eneVec = DirectX::XMLoadFloat3(enemy->getPosition());
		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(eneVec, posVec);
		DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
		float lengthSq;
		DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
		if (lengthSq < dist)
		{
			dist = lengthSq;
			ene = enemy;
		}
	}

	return ene;
}

// 敵の削除
void EnemyManager::remove(Enemy* enemy)
{
	auto itr = std::find(removes.begin(), removes.end(), enemy);

	// 重複していたら追加しない
	if (itr != removes.end())
		return;

	// 破棄リストに追加
	removes.push_back(enemy);
}