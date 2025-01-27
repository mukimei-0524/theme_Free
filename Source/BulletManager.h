#pragma once

#include "Bullet.h"
#include <vector>

// ’eŠÇ—
class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	// XVˆ—
	void update(float elpasedTime);

	// •`‰æˆ—
	void render(ID3D11DeviceContext* dc);

	// ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
	void drawDebugPrimitive();

	// ’e“o˜^
	void regist(Bullet* bullet);

	// ’e‘Síœ
	void clear();

	// ’e”æ“¾
	int getBulletCount() const { return static_cast<int>(bullets.size()); }

	// ’eæ“¾
	Bullet* getBullet(int index) { return bullets.at(index); }

	// ’eíœ
	void remove(Bullet* bullet);

private:
	std::vector<Bullet*> bullets;
	std::vector<Bullet*> removes;
};