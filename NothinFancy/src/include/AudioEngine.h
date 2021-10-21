#pragma once
#include <vector>
#include <thread>
#include <xaudio2.h>
#include <x3daudio.h>

#include "Utility.h"

namespace nf {
	class Entity;

	struct SoundData {
		WAVEFORMATEXTENSIBLE* format;
		XAUDIO2_BUFFER* buffer;
		IXAudio2SourceVoice* voice;
		bool start;
		float volume;
		bool trackToEntity;
		Entity* trackedEntity;
		bool playAtPosition;
		Vec3 position;
		bool finished = false;
	};

	class Application;

	class AudioEngine {
	public:
		AudioEngine(Application* app);

		bool isActive();
		void runAudioThread();
		void addSound(SoundData& data);
		void stopSound(const XAUDIO2_BUFFER* buffer);

		void stopAllSounds();
		~AudioEngine();
	private:
		Application* m_app;
		IXAudio2* m_engine;
		IXAudio2MasteringVoice* m_masterVoice;
		bool m_isActive;
		bool m_threadRunning;
		std::thread m_thread;
		std::vector<SoundData> m_sounds;
		bool m_clear;
	};
}