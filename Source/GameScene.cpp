#define USE_IMGUI

#include "GameScene.h"
#include "misc.h"

#include "DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include "Camera.h"

#include "EnemyManager.h"
#include "EnemySlime.h"
#include "Effect/EffectManager.h"
#include "StageManager.h"
#include "Input/InputManager.h"
#include "StageManager.h"
#include "StageMain.h"

// ����������
void GameScene::initialize()
{
	// �V�[���萔�o�b�t�@�̍쐬
	createBuffer<GameScene::SceneConstants>(DeviceManager::instance()->getDevice(), buffer.GetAddressOf());

	// �X�e�[�W�̍쐬
	StageManager* stageManager = StageManager::instance();
	StageMain* stageMain = new StageMain();
	stageManager->regist(stageMain);

	// �v���C���[�̍쐬
	player = new Player();

	// �J���������ݒ�
	DeviceManager* deviceMgr = DeviceManager::instance();
	Camera* camera = Camera::instance();
	camera->setLookAt(
		DirectX::XMFLOAT3(0, 10, -10),	// �J�����̎��_�i�ʒu�j
		DirectX::XMFLOAT3(0, 0, 0),		// �J�����̒����_�i�^�[�Q�b�g�j
		DirectX::XMFLOAT3(0, 1, 0)		// �J�����̏����
	);
	camera->setPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// ����p
		deviceMgr->getScreenWidth() / deviceMgr->getScreenHeight(),	// ��ʏc���䗦
		0.1f,		// �J�������f���o���ŋߋ���
		1000.0f		// �J�������f���o���ŉ�����
	);

	// �J��������̏�����
	cameraCtrl = new CameraController();

	EnemyManager* eneMgr = EnemyManager::instance();

	for (int i = 0; i < 2; ++i)
	{
		// �G�̍쐬�Ə�����
		EnemySlime* slime = new EnemySlime();
		slime->setPosition({ i * 2.0f,0,5 });

		// �G�Ǘ��N���X�Ɏ��t��
		eneMgr->regist(slime);
	}

	// �Q�[�W�p�X�v���C�g�inullptr ��ݒ肷��ƃ_�~�[�̃e�N�X�`�������������悤�Ɏ������Ă��܂��j
	gauge = new Sprite(deviceMgr->getDevice(), nullptr);
}

// �I������
void GameScene::finalize()
{
	// �Q�[�W�p�X�v���C�g��j��
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	// �G�̔j��
	EnemyManager::instance()->clear();

	// �J��������̔j��
	if (cameraCtrl != nullptr)
	{
		delete cameraCtrl;
		cameraCtrl = nullptr;
	}

	// �v���C���[�̔j��
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// �X�e�[�W�̔j��
	StageManager::instance()->clear();
}

// �X�V����
void GameScene::update(float elapsedTime)
{
	// �J��������̍X�V
	DirectX::XMFLOAT3 target = *(player->getPosition());
	target.y += 0.5f;	// �v���C���[�̍��̂�����ɒ����_��ݒ�
	cameraCtrl->setTarget(target);
	cameraCtrl->update(elapsedTime);

	// �X�e�[�W�X�V����
	StageManager::instance()->update(elapsedTime);

	// �v���C���[�X�V����
	player->update(elapsedTime);

	// �^�b�`�ɂ��G�̔z�u
	enemyPlacementByTouch(DeviceManager::instance()->getDeviceContext());

	// �G�X�V����
	EnemyManager::instance()->update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::instance()->update(elapsedTime);

#ifdef USE_IMGUI	
	ImGui::Begin("ImGUI");

	//ImGui::SliderFloat("speqler", &scene_constants, 0.0f, 1.0f, 0.0f);

	ImGui::End();
#endif

}

// �`�揈��
void GameScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	// 3D ���f���̕`��ɕK�v�ȏ��
	SceneConstants sc;

	{
		// �r���[�s��
		DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(camera->getView());
		// �v���W�F�N�V�����s��
		DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(camera->getProjection());

		DirectX::XMStoreFloat4x4(&sc.viewProjection, View * Projection);

		// ���C�g�����i�������j
		sc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	//TODO:IMGUI�ň�����悤�ɂ���
		sc.cameraPosition.x = camera->getEye()->x;
		sc.cameraPosition.y = camera->getEye()->y;
		sc.cameraPosition.z = camera->getEye()->z;
		sc.cameraPosition.w = 1;
	}

	// 3D �`��ݒ�
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc){
		// �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I���j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 3D �`��
	{
		// 3D �`��ɗ��p����萔�o�b�t�@�̍X�V�Ɛݒ�
		dc->UpdateSubresource(buffer.Get(), 0, 0, &sc, 0, 0);
		dc->VSSetConstantBuffers(1, 1, buffer.GetAddressOf());
		dc->PSSetConstantBuffers(1, 1, buffer.GetAddressOf());

		// �X�e�[�W�̕`��
		StageManager::instance()->render(dc);

		// �v���C���[�̕`��
		player->render(dc);

		// �G�`��
		EnemyManager::instance()->render(dc);
	}

	// 3D �G�t�F�N�g�`��
	{
		EffectManager::instance()->render(view, proj);
	}

	// 3D�f�o�b�O�`��
	{
		// �v���C���[�Փ˔���p�f�o�b�O�`��
		player->drawDrawPrimitive();

		// �G�Փ˔���p�f�o�b�O�`��
		EnemyManager::instance()->drawDebugPrimitive();

		// ���C�������_���`����s
		graphics->getLineRenderer()->render(dc, *view, *proj);

		// �f�o�b�O�����_���`����s
		graphics->getDebugRenderer()->render(dc, *view, *proj);
	}

	// 2D �`��ݒ�
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i���j�A�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I�t�A�[�x�������݃I�t�j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 2D �`��
	{
		renderEnemyGauge(dc, view, proj);
	}

	// 2D�f�o�b�OGUI�`��
	{
		// �v���C���[�̃f�o�b�O�̕`��
		player->drawDebugGui();
	}
}

// �^�b�`�ɂ��G�̔z�u
void GameScene::enemyPlacementByTouch(ID3D11DeviceContext* dc)
{
	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(proj);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	// �G�z�u����
	Mouse* mouse = InputManager::instance()->getMouse();
	if (mouse->getButtonDown() & Mouse::BTN_LEFT)
	{
		// �}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse->getPositionX());
		screenPosition.y = static_cast<float>(mouse->getPositionY());

		DirectX::XMVECTOR screenPositionVec, worldPositionVec;

		// ���C�̎n�_���v�Z
		screenPosition.z = 0.0f;	// �J�����Ɉ�ԋ߂��ʒu
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);
		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		// ���C�̏I�_���v�Z
		screenPosition.z = 1.0f;	// �J�����Ɉ�ԉ����ʒu
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		// ���C�L���X�g
		HitResult hit;
		if (StageManager::instance()->rayCast(rayStart, rayEnd, hit))
		{
			// �G��z�u
			EnemySlime* slime = new EnemySlime();
			slime->setPosition(hit.position);
			EnemyManager::instance()->regist(slime);
		}
	}

}

// �G���C�t�Q�[�W�̕`��
void GameScene::renderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4* view,
	const DirectX::XMFLOAT4X4* projection)
{
	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(projection);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	// �S�Ă̓G�̓���Ƀ��C�t�Q�[�W��\��
	EnemyManager* enemyMgr = EnemyManager::instance();
	int enemyCount = enemyMgr->getEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyMgr->getEnemy(i);
		// �G�l�~�[�̓���̃��[���h���W
		DirectX::XMFLOAT3 worldPosition = *enemy->getPosition();
		worldPosition.y += enemy->getHeight();

		DirectX::XMVECTOR worldPositionVec = DirectX::XMLoadFloat3(&worldPosition);

		// ���[���h���W����X�N���[�����W�֕ϊ�
		DirectX::XMVECTOR screenPositionVec = DirectX::XMVector3Project(
			worldPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);

		// �X�N���[�����W
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, screenPositionVec);

		// �Q�[�W�̒���
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		float healthRate = enemy->getHealth() / static_cast<float>(enemy->getMaxHealth());

		// �Q�[�W�`��
		gauge->render(
			dc,
			screenPosition.x - gaugeWidth * 0.5f,
			screenPosition.y - gaugeHeight,
			gaugeWidth * healthRate,
			gaugeHeight,
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f,
			0, 0,
			static_cast<float>(gauge->getTextureWidth()),
			static_cast<float>(gauge->getTextureHeight())
		);
	}
}