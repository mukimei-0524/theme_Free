#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

// �^�C�g���V�[��
class TitleScene : public Scene
{
public:
	TitleScene(){}
	~TitleScene() override {}

	// ����������
	void initialize() override;

	// �I������
	void finalize() override;

	// �X�V����
	void update(float elapsedTime) override;

	// �`�揈��
	void render() override;

private:
	// �^�C�g���摜
	Sprite* titleImage = nullptr;
};
