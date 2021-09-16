#include "AudioEngine.h"

#include "Application.h"

namespace nf {
	AudioEngine::AudioEngine(Application* app) :
		m_app(app),
		m_engine(nullptr),
		m_masterVoice(nullptr)
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		XAudio2Create(&m_engine, XAUDIO2_DEBUG_ENGINE);
		XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
		debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
		debug.BreakMask = XAUDIO2_LOG_ERRORS;
		m_engine->SetDebugConfiguration(&debug, 0);
		m_engine->CreateMasteringVoice(&m_masterVoice);
	}

	void AudioEngine::updateSources() {
		for (unsigned int i = 0; i < m_voices.size(); i++) {
			XAUDIO2_VOICE_STATE state;
			m_voices[i]->GetState(&state);
			if (state.BuffersQueued == 0) {
				m_voices[i]->Stop();
				m_voices[i]->FlushSourceBuffers();
				m_voices[i]->DestroyVoice();
				m_voices.erase(m_voices.begin() + i);
				i = 0;
			}
		}
	}

	IXAudio2SourceVoice* AudioEngine::getNewSourceVoice(WAVEFORMATEXTENSIBLE* fmt) {
		IXAudio2SourceVoice* s;
		HRESULT hr = m_engine->CreateSourceVoice(&s, &fmt->Format);
		m_voices.push_back(s);
		return s;
	}

	void AudioEngine::cleanup() {
		for (unsigned int i = 0; i < m_voices.size(); i++) {
			m_voices[i]->Stop();
			m_voices[i]->FlushSourceBuffers();
			m_voices[i]->DestroyVoice();
		}
		m_voices.clear();
	}

	AudioEngine::~AudioEngine() {
		cleanup();
		m_masterVoice->DestroyVoice();
		m_engine->Release();
		CoUninitialize();
	}
}