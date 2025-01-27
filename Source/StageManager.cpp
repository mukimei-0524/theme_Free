#include "StageManager.h"

// �X�V����
void StageManager::update(float elapsedTime)
{
	for (Stage* stage : stages)
	{
		stage->update(elapsedTime);
	}
}

// �`�揈��
void StageManager::render(ID3D11DeviceContext* context)
{
	for (Stage* stage : stages)
	{
		stage->render(context);
	}
}

// �X�e�[�W�S�폜
void StageManager::clear()
{
	for (Stage* stage : stages)
	{
		delete stage;
	}
	stages.clear();
}

// ���C�L���X�g
bool StageManager::rayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	hit.distance = FLT_MAX;

	for (Stage* stage : stages)
	{
		HitResult tmp;
		if (stage->rayCast(start, end, tmp))
		{
			if (hit.distance > tmp.distance)
			{
				hit = tmp;
				result = true;
			}
		}
	}

	return result;
}