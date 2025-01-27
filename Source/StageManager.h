#pragma once

#include "Stage.h"
#include <vector>

class StageManager
{
public:
	StageManager() {}
	~StageManager() {}

	// 唯一のインスタンス取得
	static StageManager* instance()
	{
		static StageManager inst;
		return &inst;
	}

	// 更新処理
	void update(float elapsedTime);

	// 描画処理
	void render(ID3D11DeviceContext* dc);

	// ステージの登録
	void regist(Stage* stage) { stages.emplace_back(stage); }

	// ステージ全削除
	void clear();

	// レイキャスト
	bool rayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

private:
	std::vector<Stage*> stages;
};