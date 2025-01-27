#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include <thread>

// ���[�f�B���O�V�[��
class LoadingScene : public Scene
{
public:
	LoadingScene(Scene* nextScene) : nextScene(nextScene) {}
	~LoadingScene() override {}

	// ����������
	void initialize() override;

	// �I������
	void finalize() override;

	// �X�V����
	void update(float elapsedTime) override;

	// �`�揈��
	void render() override;

private:
	// �ʂ̃X���b�h�Ƃ��ċN��������֐�
	static void LoadingThread(LoadingScene* scene);

private:
	// ���[�f�B���O�摜
	Sprite* loadingImage = nullptr;
	// ��]�p�x
	float angle = 0.0f;
	// ���̃V�[��
	Scene* nextScene = nullptr;
	// �}���`�p�X���b�h
	std::thread* thread = nullptr;
};