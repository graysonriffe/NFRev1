#pragma once
#include <vector>
#include <xaudio2.h>

namespace nf {
	class Application;

	class AudioEngine {
	public:
		AudioEngine(Application* app);

		void updateSources();
		IXAudio2SourceVoice* getNewSourceVoice(WAVEFORMATEXTENSIBLE* fmt);

		void cleanup();
		~AudioEngine();
	private:
		Application* m_app;
		IXAudio2* m_engine;
		IXAudio2MasteringVoice* m_masterVoice;
		std::vector<IXAudio2SourceVoice*> m_voices;
	};
}