#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>

#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

// �Q�[���V�[��
class GameScene : public Scene
{
public:
	GameScene() {}
	~GameScene() override {}

	// ����������
	void initialize() override;

	// �I������
	void finalize() override;

	// �X�V����
	void update(float elapsedTime) override;

	// �`�揈��
	void render() override;

private:
	// �^�b�`�ɂ��G�̏o��
	void enemyPlacementByTouch(ID3D11DeviceContext* dc);

	// �G���C�t�Q�[�W�`��
	void renderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4* view,
		const DirectX::XMFLOAT4X4* projection);

private:
	// �V�[���萔
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
		DirectX::XMFLOAT4		cameraPosition;
	};
	// �V�[���萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

private:
	Player* player = nullptr;
	CameraController* cameraCtrl = nullptr;
	Sprite* gauge = nullptr;
};