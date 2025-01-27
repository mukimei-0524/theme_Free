#pragma once

#include <DirectXMath.h>

// �J��������
class CameraController
{
private:
	DirectX::XMFLOAT3 target = { 0,0,0 };	// �^�[�Q�b�g�̈ʒu
	DirectX::XMFLOAT3 angle = { 0,0,0 };	// �J�����̊p�x
	float range = 10.0f;					// ��苗��

public:
	CameraController() {}
	~CameraController() {}

	// �X�V����
	void update(float elapsedTime);

	// �^�[�Q�b�g�ʒu�ݒ�
	void setTarget(const DirectX::XMFLOAT3& t) { this->target = t; }
};