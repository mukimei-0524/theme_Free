#pragma once

#include "Stage.h"
#include <vector>

class StageManager
{
public:
	StageManager() {}
	~StageManager() {}

	// �B��̃C���X�^���X�擾
	static StageManager* instance()
	{
		static StageManager inst;
		return &inst;
	}

	// �X�V����
	void update(float elapsedTime);

	// �`�揈��
	void render(ID3D11DeviceContext* dc);

	// �X�e�[�W�̓o�^
	void regist(Stage* stage) { stages.emplace_back(stage); }

	// �X�e�[�W�S�폜
	void clear();

	// ���C�L���X�g
	bool rayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

private:
	std::vector<Stage*> stages;
};