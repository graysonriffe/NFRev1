#include "AudioEngine.h"

#include "Application.h"
#include "Entity.h"

namespace nf {
	AudioEngine::AudioEngine(Application* app) :
		m_app(app),
		m_engine(nullptr),
		m_masterVoice(nullptr),
		m_isActive(false),
		m_threadRunning(false),
		m_clear(false)
	{
		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr))
			Error("Could not initialize COM!");
		hr = XAudio2Create(&m_engine);
		if (FAILED(hr))
			Error("Could not initialize the audio engine!");
#ifdef _DEBUG
		XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
		debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
		debug.BreakMask = XAUDIO2_LOG_ERRORS;
		m_engine->SetDebugConfiguration(&debug, 0);
#endif
		hr = m_engine->CreateMasteringVoice(&m_masterVoice);
		if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
			m_isActive = false;
		else if (SUCCEEDED(hr))
			m_isActive = true;
		else
			Error("Could not initialize the audio engine!");
		m_threadRunning = true;
		m_thread = std::thread(&AudioEngine::runAudioThread, this);
	}

	bool AudioEngine::isActive() {
		if (!m_isActive) {
			HRESULT hr = m_engine->CreateMasteringVoice(&m_masterVoice);
			if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
				return false;
			else if (hr == S_OK) {
				m_isActive = true;
				return true;
			}
			else {
				Error("Could not initialize audio!");
				return false;
			}
		}
		else
			return true;
	}

	void AudioEngine::runAudioThread() {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Audio Thread");
#endif
		//Wait to initialize stuff until the master voice is created if it hasn't been already
		while (!m_isActive) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		DWORD cm;
		m_masterVoice->GetChannelMask(&cm);
		X3DAUDIO_HANDLE x3d;
		X3DAudioInitialize(cm, X3DAUDIO_SPEED_OF_SOUND, x3d);
		X3DAUDIO_LISTENER listener;
		std::memset(&listener, 0, sizeof(X3DAUDIO_LISTENER));
		listener.OrientTop = X3DAUDIO_VECTOR(0.0, 1.0, 0.0);
		X3DAUDIO_EMITTER emitter;
		std::memset(&emitter, 0, sizeof(X3DAUDIO_EMITTER));
		emitter.OrientTop = X3DAUDIO_VECTOR(0.0, 1.0, 0.0);
		emitter.OrientFront = X3DAUDIO_VECTOR(0.0, 0.0, 1.0);
		emitter.CurveDistanceScaler = 1.0f;
		X3DAUDIO_DSP_SETTINGS x3dSettings;
		std::memset(&x3dSettings, 0, sizeof(X3DAUDIO_DSP_SETTINGS));
		float matrix[20] = { 0 };
		x3dSettings.pMatrixCoefficients = matrix;
		float az[20] = { 0 };
		emitter.pChannelAzimuths = az;
		XAUDIO2_FILTER_PARAMETERS filter = { LowPassFilter, 1.0, 1.0 };
		Vec3 temp;
		XAUDIO2_VOICE_STATE state;
		XAUDIO2_VOICE_DETAILS details;
		m_masterVoice->GetVoiceDetails(&details);
		unsigned int outChannels = details.InputChannels;
		x3dSettings.DstChannelCount = outChannels;

		while (m_threadRunning) {
			if (m_isActive && Application::getApp()->getCurrentState() && Application::getApp()->getCurrentState()->isRunning()) {
				//Update listener position
				temp = Application::getApp()->getCurrentState()->getCamera()->getPosition();
				listener.Position = X3DAUDIO_VECTOR(temp.x, temp.y, -temp.z);
				temp = Application::getApp()->getCurrentState()->getCamera()->getRotation();
				listener.OrientFront = X3DAUDIO_VECTOR(temp.x, 0.0f, -temp.z);

				//Stop all sounds if requested
				if (m_clear)
					stopAllSounds();

				//Update sounds
				for (SoundData& curr : m_sounds) {
					//Skip finished sounds
					if (curr.finished) continue;
					//Start sound if not started yet
					if (curr.start) {
						curr.start = false;
						IXAudio2SourceVoice* source;
						HRESULT hr = m_engine->CreateSourceVoice(&source, (WAVEFORMATEX*)curr.format, XAUDIO2_VOICE_USEFILTER);
						if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
							m_isActive = false;
							m_clear = true;
							break;
						}
						else if (!SUCCEEDED(hr))
							Error("Could not play sound!");
						curr.voice = source;
						curr.voice->SubmitSourceBuffer(curr.buffer);
						curr.voice->SetVolume(curr.volume);
						curr.voice->Start();
					}
					//Finish sound
					curr.voice->GetState(&state);
					if (state.BuffersQueued == 0) {
						curr.finished = true;
						continue;
					}

					//Update playing sound
					if (curr.playAtPosition)
						temp = curr.position;
					if (curr.trackToEntity)
						temp = curr.trackedEntity->getPosition();

					if (curr.playAtPosition || curr.trackToEntity) {
						int ch = curr.format->Format.nChannels;
						emitter.ChannelCount = ch;
						x3dSettings.SrcChannelCount = ch;
						emitter.Position = X3DAUDIO_VECTOR(temp.x, temp.y, -temp.z);
						X3DAudioCalculate(x3d, &listener, &emitter, X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB, &x3dSettings);
						float temp2 = matrix[1];
						matrix[1] = matrix[2];
						matrix[2] = temp2;
						curr.voice->SetOutputMatrix(m_masterVoice, ch, outChannels, matrix);
						curr.voice->SetFrequencyRatio(x3dSettings.DopplerFactor);
						filter.Frequency = 2.0f * std::sinf(X3DAUDIO_PI / 6.0f * x3dSettings.LPFDirectCoefficient);
						curr.voice->SetFilterParameters(&filter);
					}
				}

				//Delete all finished sounds from the list
				for (size_t i = 0; i < m_sounds.size(); i++) {
					if (m_sounds[i].finished && m_sounds[i].voice) {
						m_sounds[i].voice->Stop();
						m_sounds[i].voice->FlushSourceBuffers();
						m_sounds[i].voice->DestroyVoice();
						m_sounds.erase(m_sounds.begin() + i);
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}

		//Cleanup
		stopAllSounds();
		m_masterVoice->DestroyVoice();
	}

	void AudioEngine::addSound(SoundData& data) {
		m_sounds.push_back(data);
	}

	void AudioEngine::stopSound(const XAUDIO2_BUFFER* buffer) {
		//Maybe should move to audio thread somehow?
		for (SoundData& curr : m_sounds) {
			if (std::memcmp(curr.buffer, buffer, sizeof(XAUDIO2_BUFFER)) == 0)
				curr.finished = true;
		}
	}

	void AudioEngine::stopAllSounds() {
		m_clear = false;
		for (SoundData& curr : m_sounds) {
			if (curr.start) continue;
			curr.voice->Stop();
			curr.voice->FlushSourceBuffers();
			curr.voice->DestroyVoice();
		}
		m_sounds.clear();
	}

	AudioEngine::~AudioEngine() {
		m_threadRunning = false;
		m_thread.join();
		m_engine->Release();
		CoUninitialize();
	}
}