#include "CameraController.h"
#include "Camera.h"
#include "Input/InputManager.h"

// 更新処理
void CameraController::update(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	float ax = gamePad->getAxisRX();
	float ay = gamePad->getAxisRY();

	// カメラの回転速度
	constexpr float rollSpeed = DirectX::XMConvertToRadians(90);
	float speed = rollSpeed * elapsedTime;

	// スティックの入力値に合わせて回転
	angle.x += ay * speed;	// X 軸回転
	angle.y += ax * speed;	// Y 軸回転

	// X軸のカメラ回転を上下45度に制限
	constexpr float maxAngle = DirectX::XMConvertToRadians(45);
	constexpr float minAngle = DirectX::XMConvertToRadians(-45);

	// X 軸のカメラ回転を制限
	if (angle.x < minAngle)
		angle.x = minAngle;

	if (angle.x > maxAngle)
		angle.x = maxAngle;

	// Y 軸の回転値を調整
	if (angle.y < -DirectX::XM_PI)
		angle.y += DirectX::XM_2PI;

	if (angle.y > DirectX::XM_PI)
		angle.y -= DirectX::XM_2PI;

	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x ,angle.y, angle.z);

	// カメラの回転行列から前方向ベクトルを取り出す
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Transform.r[2]);

	// 注視点から後ろベクトル方向に一定距離れたカメラ視点を求める（eye = target - front * range）
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	// カメラの視点と注視点を設定
	Camera::instance()->setLookAt(eye, target, { 0,1,0 });
}