#include "Graphics/GraphicsManager.h"
#include "Input/InputManager.h"
#include "Graphics/ImGuiRenderer.h" 
#include "DeviceManager.h"
#include "EnemyManager.h"
#include "StraightBullet.h"
#include "Collision.h"
#include "Player.h"
#include "Camera.h"

// �R���X�g���N�^
Player::Player()
{
	model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Jummo\\Jummo.cereal");

	const float scale_fcator = 0.01f;	// ���f�����傫���̂ŃX�P�[������
	scale = { scale_fcator, scale_fcator, scale_fcator };

	// �q�b�g�G�t�F�N�g�ǂݍ���
	hitEffect = new Effect(".\\Resources\\Effect\\01_AndrewFM01\\hit_eff.efk");
}

// �f�X�g���N�^
Player::~Player()
{
	if (hitEffect != nullptr)
	{
		delete hitEffect;
		hitEffect = nullptr;
	}

	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

// �X�V����
void Player::update(float elapsedTime)
{
	// �ړ����͏���
	inputMove(elapsedTime);

	// �W�����v���͏���
	inputJump();

	// ���͂ɂ��e���ˏ���
	inputLaunchBullet();

	// ���x�����X�V
	updateVelocity(elapsedTime);

	// �e�X�V����
	bulletMgr.update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ��u
	collisionPlayerAndEnemies();

	// �e�ƓG�̏Փˏ���
	collisionBulletsAndEnemies();

	// ���[���h�s��̍X�V
	updateTransform();
}

// �`�揈��
void Player::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);

	// �e�`�揈��
	bulletMgr.render(dc);
}

// �f�o�b�O�p�̕`��
void Player::drawDebugGui()
{
	ImGui::Begin("Player");

	// �ʒu
	ImGui::InputFloat3("Position", &position.x);

	// ��]
	DirectX::XMFLOAT3 a{ DirectX::XMConvertToDegrees(angle.x), DirectX::XMConvertToDegrees(angle.y) , DirectX::XMConvertToDegrees(angle.z) };
	ImGui::InputFloat3("Angle", &a.x);
	angle = { DirectX::XMConvertToRadians(a.x),DirectX::XMConvertToRadians(a.y), DirectX::XMConvertToRadians(a.z) };

	// �g��k��
	ImGui::InputFloat3("Scale", &scale.x);

	ImGui::End();
}

// �f�o�b�O�v���~�e�B�u�̕`��
void Player::drawDrawPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	// �Փ˔���p�̃f�o�b�O�p�~����`��
	debugRenderer->drawCylinder(position, radius, height, { 0, 0, 0, 1 });

	// �e�f�o�b�O�v���~�e�B�u�`��
	bulletMgr.drawDebugPrimitive();
}

// ���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::getMoveVec() const
{
	// ���͏����擾
	GamePad* gamePad = InputManager::instance()->getGamePad();
	float ax = gamePad->getAxisLX();
	float ay = gamePad->getAxisLY();

	// �J�����������擾
	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT3* cameraFront = camera->getFront();
	const DirectX::XMFLOAT3* cameraRight = camera->getRight();

	// �J�����O�����x�N�g���� XZ �P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront->x;
	float cameraFrontZ = cameraFront->z;

	// �J�����O�����x�N�g����P�ʃx�N�g����
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		cameraFrontX = cameraFrontX / cameraFrontLength;
		cameraFrontZ = cameraFrontZ / cameraFrontLength;
	}

	// �J�����E�����x�N�g���� XZ �P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight->x;
	float cameraRightZ = cameraRight->z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		cameraRightX = cameraRightX / cameraRightLength;
		cameraRightZ = cameraRightZ / cameraRightLength;
	}

	// �������͒l���J�����O�����ɔ��f���i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = cameraFrontX * ay + cameraRightX * ax;
	vec.z = cameraFrontZ * ay + cameraRightZ * ax;

	return vec;
}


// �ړ����͏���
void Player::inputMove(float elapsedTime)
{
	// �i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = getMoveVec();

	// �ړ�����
	move(moveVec.x, moveVec.z, moveSpeed);

	// ���񏈗�
	turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}



// �v���C���[�ƓG�Ƃ̏Փˏ���
void Player::collisionPlayerAndEnemies()
{
	EnemyManager* eneMgr = EnemyManager::instance();

	// �S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = eneMgr->getEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = eneMgr->getEnemy(i);

		// �Փ˕���
		DirectX::XMFLOAT3 outVec;

		// �Փˏ���
		if (Collision::intersectCylinderAndCylinder(
			position,
			radius,
			height,
			*(enemy->getPosition()),
			enemy->getRadius(),
			enemy->getHeight(),
			outVec
		))
		{
			OutputDebugStringA("�Փ�\n");

			// �G�̐^��t�߂ɂ����������𔻒�
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(enemy->getPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			// �ォ�瓥��Â����ꍇ�͏��W�����v����
			if (normal.y > 0.8f)
			{
				// ���W�����v����
				jump(jumpSpeed * 0.5f);
			}
			else
			{
				// �����o����̈ʒu�ݒ�

				// ���a�̍��v
				float range = radius + enemy->getRadius();

				// �Փ˕����̃x�N�g���𔼌a�̍��v�̒����ɃX�P�[�����O
				DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

				// ���̃x�N�g�����Փ˂��鑤�̈ʒu�ɑ���
				vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), vec);

				// ���̈ʒu�x�N�g�����Փ˂���鑤�ɐݒ肷��
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, vec);
				pos.y = enemy->getPosition()->y;
				enemy->setPosition(pos);
			}
		}
	}
}

// �e�ƓG�̏Փˏ���
void Player::collisionBulletsAndEnemies()
{
	EnemyManager* enemyMgr = EnemyManager::instance();

	// �S�Ă̒e�ƑS�Ă̓G�𑍓�����ŏՓˏ���
	int bulletCount = bulletMgr.getBulletCount();
	int enemyCount = enemyMgr->getEnemyCount();
	for (int i = 0; i < bulletCount; ++i)
	{
		Bullet* bullet = bulletMgr.getBullet(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyMgr->getEnemy(j);

			// �Փˏ���
			DirectX::XMFLOAT3 outVec;
			if (Collision::intersectSphereAndCylinder(
				*bullet->getPosition(),
				bullet->getRadius(),
				*enemy->getPosition(),
				enemy->getRadius(),
				enemy->getHeight(),
				outVec))
			{
				int damage = 1;
				if (enemy->applyDamage(damage, 0.1f))
				{
					// ������΂�
					float power = 10.0f;
					DirectX::XMFLOAT3 impulse;
					impulse.x = outVec.x * power;
					impulse.y = power * 0.5f;
					impulse.z = outVec.z * power;
					enemy->addImpulse(impulse);

					// �q�b�g�G�t�F�N�g�̍Đ�
					DirectX::XMFLOAT3 enePos = *enemy->getPosition();
					enePos.y += enemy->getHeight() * 0.5f;
					Effekseer::Handle handle = hitEffect->play(&enePos, 0.5f);


					// �e�̔j��
					bullet->destroy();
				}
			}
		}
	}
}

// �W�����v���͏���
void Player::inputJump()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	if (gamePad->getButtonDown() & GamePad::BTN_A)
	{
		// �W�����v�񐔐���
		if (jumpCount < jumpLimit)
		{
			// �W�����v
			jumpCount++;
			jump(jumpSpeed);
		}
	}
}

// ���͂ɂ��e���ˏ���
void Player::inputLaunchBullet()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();

	// �X�g���[�g�e����
	if (gamePad->getButtonDown() & GamePad::BTN_X)
	{
		// �O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		// ���ˈʒu�i�v���C���[�̍�������
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		// ����
		StraightBullet* bullet = new StraightBullet(&bulletMgr);
		bullet->setDirection(dir);
		bullet->setPosition(pos);
	}
}

// ���n���ɌĂяo�����
void Player::onLanding()
{
	jumpCount = 0;
}