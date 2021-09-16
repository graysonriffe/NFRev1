#include "Sound.h"

#include "Application.h"
#include "Assets.h"
#include "Utility.h"

namespace nf {
	Sound::Sound() :
		m_constructed(false),
		m_dataSize(0),
		m_volume(1.0f),
		m_format({ 0 }),
		m_buffer(nullptr),
		m_currentVoice(nullptr)
	{

	}

	void Sound::create(Asset* soundAsset) {
		if (m_constructed)
			Error("Sound already created!");
		m_constructed = true;
		ASound* sound;
		if ((sound = dynamic_cast<ASound*>(soundAsset)) == nullptr)
			Error("Non-sound asset passed to Sound::create!");
		std::string data(sound->data, sound->size);
		if(data.find("RIFF") == std::string::npos)
			Error("Sound asset not of correct format!");
		unsigned int fileSize = *(unsigned int*)&data[4];
		unsigned int fmtPos;
		if((fmtPos = data.find("fmt")) == std::string::npos)
			Error("Sound asset not of correct format!");
		std::memcpy(&m_format, &data[fmtPos + 8], 16);
		unsigned int dataPos;
		if ((dataPos = data.find("data")) == std::string::npos)
			Error("Sound asset not of correct m_format!");
		m_dataSize = *(unsigned int*)&data[dataPos + 4];
		m_buffer = new unsigned char[m_dataSize];
		std::memcpy(m_buffer, &data[dataPos + 8], m_dataSize);

		Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
	}

	void Sound::setVolume(double volume) {
		m_volume = (float)volume;
	}

	void Sound::play(bool loop) {
		m_currentVoice = Application::getApp()->getAudioEngine()->getNewSourceVoice(&m_format);
		m_currentVoice->SetVolume(m_volume);

		XAUDIO2_BUFFER xBuffer = { 0 };
		xBuffer.pAudioData = m_buffer;
		xBuffer.AudioBytes = m_dataSize;
		if (loop)
			xBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		m_currentVoice->SubmitSourceBuffer(&xBuffer);
		m_currentVoice->Start();
	}

	void Sound::stop() {
		if (m_currentVoice) {
			XAUDIO2_VOICE_STATE state;
			m_currentVoice->GetState(&state);
			if (state.BuffersQueued > 0) {
				m_currentVoice->Stop();
				m_currentVoice->FlushSourceBuffers();
				m_currentVoice = nullptr;
			}
		}
	}

	void Sound::destroy() {
		m_constructed = false;
		m_dataSize = 0;
		m_volume = 1.0f;
		m_format = { 0 };
		delete m_buffer;
		m_currentVoice = nullptr;
	}

	Sound::~Sound() {
		destroy();
	}
}