#include "Sound.h"

#include "Application.h"
#include "Assets.h"
#include "Entity.h"
#include "Utility.h"

namespace nf {
	Sound::Sound() :
		m_constructed(false),
		m_dataSize(0),
		m_volume(1.0f),
		m_usePos(false),
		m_format({ 0 }),
		m_buffer(nullptr),
		m_currentVoice(nullptr),
		m_targetEntity(nullptr),
		m_soundPos(0.0)
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
		m_dataSize = *(unsigned int*)&data[dataPos + 4];
		m_buffer = new unsigned char[m_dataSize];
		std::memcpy(m_buffer, &data[dataPos + 8], m_dataSize);

		m_emitter = { 0 };
		m_emitter.ChannelCount = 2;
		m_emitter.CurveDistanceScaler = 1.0;

		Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
	}

	void Sound::setVolume(double volume) {
		m_volume = (float)volume;
	}

	void Sound::setEntity(Entity& entity) {
		m_targetEntity = &entity;
		m_usePos = false;
	}

	void Sound::setPosition(const Vec3& position) {
		m_soundPos = position;
		m_usePos = true;
	}

	void Sound::play(bool loop) {
		m_currentVoice = Application::getApp()->getAudioEngine()->getNewSourceVoice(&m_format);
		m_currentVoice->SetVolume(m_volume);

		XAUDIO2_BUFFER xBuffer = { 0 };
		xBuffer.pAudioData = m_buffer;
		xBuffer.AudioBytes = m_dataSize;
		if (loop)
			xBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;

		if (m_usePos || m_targetEntity) {
			if (m_usePos)
				m_emitter.Position = X3DAUDIO_VECTOR((float)m_soundPos.x, (float)m_soundPos.y, (float)-m_soundPos.z);
			else if (m_targetEntity) {
				Vec3 temp = m_targetEntity->getPosition();
				m_emitter.Position = X3DAUDIO_VECTOR((float)temp.x, (float)temp.y, (float)-temp.z);
			}
			m_emitter.OrientFront = X3DAUDIO_VECTOR(0.0, 0.0, 1.0);
			m_emitter.OrientTop = X3DAUDIO_VECTOR(0.0, 1.0, 0.0);
			float az[2] = { 0 };
			m_emitter.pChannelAzimuths = az;
			Vec3 temp = Application::getApp()->getCurrentState()->getCamera()->getPosition();
			m_listener.Position = X3DAUDIO_VECTOR((float)temp.x, (float)temp.y, (float)-temp.z);
			temp = Application::getApp()->getCurrentState()->getCamera()->getRotation();
			m_listener.OrientFront = X3DAUDIO_VECTOR((float)temp.x, 0.0f, (float)-temp.z);
			m_listener.OrientTop = X3DAUDIO_VECTOR(0.0, 1.0, 0.0);
			X3DAUDIO_DSP_SETTINGS settings = { 0 };
			settings.SrcChannelCount = 2;
			settings.DstChannelCount = 2;
			float matrix[4] = { 0 };
			settings.pMatrixCoefficients = matrix;

			IXAudio2MasteringVoice* master = Application::getApp()->getAudioEngine()->getMasterVoice();
			X3DAUDIO_HANDLE* instance = Application::getApp()->getAudioEngine()->getX3DAudioInstance();
			X3DAudioCalculate(*instance, &m_listener, &m_emitter, X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB, &settings);
			float temp2 = settings.pMatrixCoefficients[1];
			settings.pMatrixCoefficients[1] = settings.pMatrixCoefficients[2];
			settings.pMatrixCoefficients[2] = temp2;
			m_currentVoice->SetOutputMatrix(master, 2, 2, settings.pMatrixCoefficients);
			m_currentVoice->SetFrequencyRatio(settings.DopplerFactor);
			XAUDIO2_FILTER_PARAMETERS lpf = { LowPassFilter, 2.0f * std::sinf(X3DAUDIO_PI / 6.0f * settings.LPFDirectCoefficient), 1.0f };
			m_currentVoice->SetFilterParameters(&lpf);
		}

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
		if (m_constructed) {
			delete m_buffer;
			m_buffer = nullptr;
		}
		m_constructed = false;
		m_dataSize = 0;
		m_volume = 1.0f;
		m_format = { 0 };
		m_currentVoice = nullptr;
	}

	Sound::~Sound() {
		destroy();
	}
}