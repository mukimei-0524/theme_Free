#include "StageMain.h"
#include "DeviceManager.h"

// コンストラクタ
StageMain::StageMain()
{
	model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\ExampleStage\\ExampleStage.cereal");
}

// デストラクタ
StageMain::~StageMain()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

// 更新処理
void StageMain::update(float elapsedTime)
{
	// 今は特に処理はなし
}

// 描画処理
void StageMain::render(ID3D11DeviceContext* dc)
{
	model->render(dc, { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }, {0.8f,0.8f,0.8f,1.0f}, nullptr);
}

// レイキャスト
bool StageMain::rayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::intersectRayAndModel(start, end, model, hit);
}
