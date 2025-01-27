#pragma once

#include <memory>
#include <xaudio2.h>
#include "AudioResource.h"

// �I�[�f�B�I
class Audio
{
public:
	Audio(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource);
	~Audio();

	/// <summary>
	/// �I�[�f�B�I�̍Đ�
	/// </summary>
	/// <param name="loop">true...���[�v</param>
	void play(bool loop);

	/// <summary>
	/// �I�[�f�B�I�̒�~
	/// </summary>
	void stop();

private:
	IXAudio2SourceVoice*			sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;
};
