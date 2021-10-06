#pragma once
#include <vector>
#include <xaudio2.h>
#include <x3daudio.h>

namespace nf {
	class Application;

	class AudioEngine {
	public:
		AudioEngine(Application* app);

		void updateSources();
		IXAudio2SourceVoice* getNewSourceVoice(WAVEFORMATEXTENSIBLE* fmt);
		IXAudio2MasteringVoice* getMasterVoice();
		X3DAUDIO_HANDLE* getX3DAudioInstance();

		void cleanup();
		~AudioEngine();
	private:
		Application* m_app;
		IXAudio2* m_engine;
		X3DAUDIO_HANDLE m_x3d;
		IXAudio2MasteringVoice* m_masterVoice;
		std::vector<IXAudio2SourceVoice*> m_voices;
	};
}