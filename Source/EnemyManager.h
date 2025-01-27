#pragma once

#include <vector>
#include "Enemy.h"

// “GŠÇ—
class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}

public:
	// singleton
	static EnemyManager* instance()
	{
		static EnemyManager inst;
		return &inst;
	}

	// XVˆ—
	void update(float elapsedTime);

	// •`‰æˆ—
	void render(ID3D11DeviceContext* dc);

	// “G‚Ì“o˜^
	void regist(Enemy* enemy);

	// “G‚Ì”jŠü
	void clear();

	// ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
	void drawDebugPrimitive();

	// “G‚Ì‘”‚ğæ“¾
	int getEnemyCount() const { return static_cast<int>(enemies.size()); }

	// “G‚ğæ“¾
	Enemy* getEnemy(int index) { return enemies.at(index); }

	// ˆê”Ô‹ß‚­‚Ì“G‚ğæ“¾
	Enemy* searchEnemy(DirectX::XMFLOAT3* position);

	// “G‚Ìíœ
	void remove(Enemy* enemy);

private:
	// “G“¯m‚ÌÕ“Ëˆ—
	void collisoinEnemiesAndEnemies();

private:
	std::vector<Enemy*>	enemies;
	std::vector<Enemy*> removes;
};