#pragma once

// �V�[��
class Scene
{
public:
	Scene(){}
	virtual ~Scene(){}

	// ����������
	virtual void initialize() = 0;

	// �I������
	virtual void finalize() = 0;

	// �X�V����
	virtual void update(float elapsedTime) = 0;

	// �`�揈��
	virtual void render() = 0;

	// true...��������
	bool isReady() const { return readyFlag; }

	// ���������ݒ�
	void setReady() { readyFlag = true; }

private:
	// true...��������
	bool readyFlag = false;
};