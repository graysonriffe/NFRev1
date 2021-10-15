#include "Sound.h"

#include "Application.h"
#include "Assets.h"
#include "Entity.h"

namespace nf {
	Sound::Sound() :
		m_constructed(false),
		m_volume(1.0f),
		m_usePos(false),
		m_useEntity(false),
		m_format({ 0 }),
		m_xBuffer({ 0 }),
		m_buffer(nullptr),
		m_targetEntity(nullptr)
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
		if (data.find("RIFF") == std::string::npos)
			Error("Sound asset not of correct format!");
		unsigned int fileSize = *(unsigned int*)&data[4];
		size_t fmtPos;
		if ((fmtPos = data.find("fmt")) == std::string::npos)
			Error("Sound asset not of correct format!");
		std::memcpy(&m_format, &data[fmtPos + 8], 16);
		size_t dataPos;
		if ((dataPos = data.find("data")) == std::string::npos)
			Error("Sound asset not of correct m_format!");
		unsigned int dataSize = *(unsigned int*)&data[dataPos + 4];
		m_buffer = new unsigned char[dataSize];
		std::memcpy(m_buffer, &data[dataPos + 8], dataSize);
		m_xBuffer.pAudioData = m_buffer;
		m_xBuffer.AudioBytes = dataSize;
		m_xBuffer.Flags = XAUDIO2_END_OF_STREAM;

		if (!Application::getApp()->getCurrentState()->isRunning())
			Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
	}

	void Sound::setVolume(double volume) {
		m_volume = (float)volume;
	}

	void Sound::setEntity(Entity& entity) {
		m_targetEntity = &entity;
		m_useEntity = true;
		m_usePos = false;
	}

	void Sound::setPosition(const Vec3& position) {
		m_soundPos = position;
		m_usePos = true;
		m_useEntity = false;
	}

	void Sound::play(bool loop) {
		if (!Application::getApp()->getAudioEngine()->isActive()) return;

		if (loop)
			m_xBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;

		SoundData sd = { &m_format, &m_xBuffer, nullptr, true, m_volume, m_useEntity, m_targetEntity, m_usePos, m_soundPos };
		Application::getApp()->getAudioEngine()->addSound(sd);
	}

	void Sound::stop() {
		Application::getApp()->getAudioEngine()->stopSound(&m_xBuffer);
	}

	void Sound::destroy() {
		if (m_constructed) {
			delete m_buffer;
			m_buffer = nullptr;
		}
		m_constructed = false;
		m_volume = 1.0f;
		m_format = { 0 };
	}

	Sound::~Sound() {
		destroy();
	}
}