#include "SceneManager.h"

// �X�V����
void SceneManager::update(float elapsedTime)
{
	// �u���̃V�[���v���ݒ肳��Ă��邩�H
	if (nextScene != nullptr)
	{
		// �u���݂̃V�[���v��j��
		clear();

		// �u���̃V�[���v���u���݂̃V�[���v�Ƃ��Đݒ�
		currentScene = nextScene;

		// �u���̃V�[���v�� nullptr ����
		nextScene = nullptr;

		// �V�[����������
		if (currentScene->isReady() == false)
		{
			currentScene->initialize();
		}
	}

	if (currentScene != nullptr)
	{
		currentScene->update(elapsedTime);
	}
}

// �`�揈��
void SceneManager::render()
{
	if (currentScene != nullptr)
	{
		currentScene->render();
	}
}

// ���݂̃V�[�����N���A
void SceneManager::clear()
{
	if (currentScene != nullptr)
	{
		currentScene->finalize();
		delete currentScene;
		currentScene = nullptr;
	}
}

// �V�[���̐؂�ւ��i�O���Ő��������V�[�������̃V�[���Ƃ��Đݒ�j
void SceneManager::changeScene(Scene* scene)
{
	// ���̃V�[���Ƃ��Đݒ�
	nextScene = scene;
}