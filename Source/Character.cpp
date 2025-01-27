#include "Character.h"
#include "StageManager.h"
#include "MathCulc.h"

// �s��X�V����
void Character::updateTransform()
{
	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// ��]�s����쐬
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;
	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// �R�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	// �v�Z�������[���h�s��� transform �Ɏ��o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

/// <summary>
/// �_���[�W����
/// </summary>
/// <param name="damage">�_���[�W��</param>
/// <param name="timer">���G����</param>
/// <returns>true...HP�ɕω���������</returns>
bool Character::applyDamage(int damage, float timer)
{
	// �_���[�W���O�ȉ��̏ꍇ�� HP �ɕω��Ȃ�
	if (damage <= 0)
		return false;

	// ���� HP ���O�ȉ��i���S�j�̏ꍇ�� HP �ɕω��Ȃ�
	if (health <= 0)
		return false;

	// ���G���Ԓ��� HP �ɕω��Ȃ�
	if (invincibleTimer > 0.0f)
		return false;

	// �_���[�W���󂯂��ۂɖ��G���Ԃ��Đݒ�
	invincibleTimer = timer;

	// �_���[�W�v�Z
	health -= damage;

	// �_���[�W�v�Z��̎c�� HP �ŏ�����ω�
	if (health <= 0)
	{
		// ���S
		onDead();
	}
	else
	{
		// �_���[�W���󂯂�
		onDamaged();
	}

	return true;
}

// �Ռ���^����
void Character::addImpulse(const DirectX::XMFLOAT3& impulse)
{
	// ���x�ɗ͂�������
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

// �ړ�����������
void Character::move(float vx, float vz, float speed)
{
	// �ړ������x�N�g��������
	direction.x = vx;
	direction.z = vz;

	// �ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

// ���񏈗�
void Character::turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	// �i�ׂ��i�s�x�N�g�����[���̏ꍇ�͐��񏈗��̕K�v�Ȃ�
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f)
		return;

	// �i�s�x�N�g���ƒP�ʃx�N�g����
	vx /= length;
	vz /= length;

	// �O�����x�N�g���� XZ �������擾���P�ʃx�N�g����
	float frontX = transform._31;
	float frontZ = transform._33;
	float frontLength = sqrtf(frontX * frontX + frontZ * frontZ);
	if (frontLength > 0.0f)
	{
		frontX /= frontLength;
		frontZ /= frontLength;
	}

	// ���E�`�F�b�N�ׂ̈̊O�όv�Z
	float cross = (frontZ * vx) - (frontX * vz);


	// ��]�p�����߂�ׁA�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	// �Q�̃x�N�g���̓��ϒl�� -1.0 �` 1.0 �ŕ\������܂��B
	float dot = (frontX * vx) + (frontZ * vz);

	// �Q�̃x�N�g���̊p�x���������قǁi�߂Â����j 1.0 �ɋ߂Â��܂��B
	// �Q�̃x�N�g���̊p�x���傫���قǁi�������j -1.0 �ɋ߂Â��܂��B
	// �܂�v���C���[�̑O�����Ɛi�s�����x�N�g�����߂Â��ق� dot �� 1.0 �ɋ߂Â�
	// �v�Z�Ƃ��ĉ�]���x�ł��� rot �� 0.0f �ɋ߂Â�
	// �d�Ȃ������_�ŉ�]���x�� 0.0f �ɂȂ�
	float rot = (1.0f - dot);

	// ���܂藣�ꂷ����Ɖ�]���x�������Ȃ肷����̂� speed �ȏ�̉�]���x�ɂ͂Ȃ�Ȃ��悤����
	if (rot > speed)
		rot = speed;

	// �O�ς����̏ꍇ�͉E��]�A���̏ꍇ�͍���]
	if (cross < 0.0f)
	{
		angle.y -= rot;
	}
	else
	{
		angle.y += rot;
	}
}

// �W�����v����
void Character::jump(float speed)
{
	// ������̗͂�ݒ�
	velocity.y = speed;
}

// ���x�����X�V
void Character::updateVelocity(float elapsedTime)
{
	// �������x�X�V����
	updateVerticalVelocity(elapsedTime);

	// �����ړ��X�V����
	updateVerticalMove(elapsedTime);

	// �������x�X�V����
	updateHorizontalVelocity(elapsedTime);

	// �����ړ��X�V����
	updateHorizontalMove(elapsedTime);
}

// ���G���Ԃ̍X�V
void Character::updateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

// �������x�X�V����
void Character::updateVerticalVelocity(float elapsedTime)
{
	// �d�͏����i�t���[���P�ʂŌv�Z�j
	velocity.y += gravity * elapsedTime * 60.0f;
}

// �����ړ��X�V����
void Character::updateVerticalMove(float elapsedTime)
{
	// ���������̈ړ���
	float moveY = velocity.y * elapsedTime;

	// �p������p�@���x�N�g���i�f�t�H���g�͏�����j
	DirectX::XMFLOAT3 normal = { 0,1,0 };

	if (moveY < 0.0f)
	{
		// ������

		// ���C�̊J�n�ʒu��ݒ�i�����Ƃ�菭����j
		DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
		// ���C�̏I�_�ʒu��ݒ�i�ړ���̈ʒu�j
		DirectX::XMFLOAT3 end = { position.x, position.y + moveY, position.z };

		// ���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::instance()->rayCast(start, end, hit))
		{
			// �n�ʂ̌������p������p�@���x�N�g���ɐݒ�
			normal = hit.normal;

			// �n�ʂɐݒu���Ă���i�q�b�g���ʂ� y �ʒu�𔽉f�j
			position.y = hit.position.y;
			velocity.y = 0.0f;

			// �X�Η��̌v�Z
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			// ���n����
			if (groundedFlag == false)
			{
				onLanding();
			}
			groundedFlag = true;

		}
		else
		{
			// �󒆂ɕ����Ă���
			position.y += moveY;
			groundedFlag = false;
		}
	}
	else if (moveY > 0.0f)
	{
		// �㏸��
		position.y += moveY;
		groundedFlag = false;
	}

	// �p������p�@���x�N�g������ x �� z �̊p�x���v�Z
	// y �����p������p�@���x�N�g�������Ɍ����悤�Ɋp�x���v�Z���܂�
	float angleX = atan2f(normal.z, normal.y);
	float angleZ = -atan2f(normal.x, normal.y);

	// ���`��ԂŊ��炩�ɉ�]
	angle.x = MathCulc::Lerp(angle.x, angleX, 0.2f);
	angle.z = MathCulc::Lerp(angle.z, angleZ, 0.2f);
}

// �������x�X�V����
void Character::updateHorizontalVelocity(float elapsedTime)
{
	// ���x�ɗ͂�������Ă�����i0 ����Ȃ�������j�����������s��
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		// ���C�́i�t���[���P�ʂŌv�Z�j
		float friction = this->friction * elapsedTime * 60.0f;

		// �󒆂ɂ���Ƃ��͖��C�͂�����
		if (groundedFlag == false)
			friction *= airControl;

		// ���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		else
		{
			// �������̑��x�����C�͈ȉ��ɂȂ����̂ő��x�𖳌���
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	// �ő呬�x�ȉ��Ȃ�����������s��
	if (length <= maxMoveSpeed)
	{
		// �����x�N�g�����[���łȂ��Ȃ�����������s��
		// �[���̏ꍇ�͓��͂���Ă��Ȃ�
		float directionLength = sqrtf(direction.x * direction.x + direction.z * direction.z);
		if (directionLength > 0.0f)
		{
			// �����x�i�t���[���P�ʂŌv�Z�j
			float acceleration = this->acceleration * elapsedTime * 60.0f;
		
			// �󒆂ɂ���Ƃ��͉����x������
			if (groundedFlag == false)
				acceleration *= airControl;

			// ��������
			// �����x�N�g���ɉ����x���X�P�[�����O�����l�𑬓x�x�N�g���ɉ��Z
			velocity.x += direction.x * acceleration;
			velocity.z += direction.z * acceleration;

			// �X�Η��������ꍇ�͗����Ă����悤�Ɍv�Z
			if (groundedFlag == true && slopeRate > 0.0f)
				velocity.y -= length * slopeRate * elapsedTime * 60.0f;

			// �ő呬�x����
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				// �����x�N�g���ɍő�ړ����x���X�P�[�����O�����l�𑬓x�x�N�g���ɑ��
				velocity.x = direction.x * maxMoveSpeed;
				velocity.z = direction.z * maxMoveSpeed;
			}
		}
	}
}

// �����ړ��X�V����
void Character::updateHorizontalMove(float elapsedTime)
{
	// �������x�ʌv�Z
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		// �v�Z�p�̈ړ���̑��x
		float moveX = velocity.x * elapsedTime;
		float moveZ = velocity.z * elapsedTime;

		// ���C�̎n�_�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
		DirectX::XMFLOAT3 end = { position.x + moveX, position.y + 1.0f, position.z + moveZ };

		// ���C�L���X�g
		HitResult hit;
		if (StageManager::instance()->rayCast(start, end, hit))
		{
			// �I�_����n�_�ւ̃x�N�g�����擾
			DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&hit.position);
			DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(startVec, endVec);

			// �ǂ̖@���x�N�g��
			DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&hit.normal);

			// ���˃x�N�g����@���Ŏˉe
			DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
			float length;
			DirectX::XMStoreFloat(&length, lengthVec);

			// �␳�ʒu�ւ̃x�N�g�����v�Z
			DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

			// �ŏI�I�ȕ␳�ʒu�x�N�g�����v�Z
			DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
			DirectX::XMFLOAT3 correctPos;
			DirectX::XMStoreFloat3(&correctPos, correctPosVec);


			HitResult hit2;
			if (StageManager::instance()->rayCast(hit.position, correctPos, hit2))
			{
				position.x = hit2.position.x;
				position.z = hit2.position.z;
			}
			else
			{
				position.x = correctPos.x;
				position.z = correctPos.z;
			}
		}
		else
		{
			// �ǂɂ������Ă��Ȃ��ꍇ�͒ʏ�̈ړ�
			position.x += moveX;
			position.z += moveZ;
		}
	}
}