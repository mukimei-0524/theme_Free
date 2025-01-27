#include "StageMain.h"
#include "DeviceManager.h"

// �R���X�g���N�^
StageMain::StageMain()
{
	model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\ExampleStage\\ExampleStage.cereal");
}

// �f�X�g���N�^
StageMain::~StageMain()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

// �X�V����
void StageMain::update(float elapsedTime)
{
	// ���͓��ɏ����͂Ȃ�
}

// �`�揈��
void StageMain::render(ID3D11DeviceContext* dc)
{
	model->render(dc, { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }, {0.8f,0.8f,0.8f,1.0f}, nullptr);
}

// ���C�L���X�g
bool StageMain::rayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::intersectRayAndModel(start, end, model, hit);
}
