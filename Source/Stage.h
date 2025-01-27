#pragma once

#include <d3d11.h>
#include "Collision.h"

// ステージ
class Stage
{
public:
	Stage() {}
	virtual ~Stage() {}

	// 更新処理
	virtual void update(float elapsedTime) = 0;

	// 描画処理
	virtual void render(ID3D11DeviceContext* dc) = 0;

	// レイキャスト
	virtual bool rayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;
};