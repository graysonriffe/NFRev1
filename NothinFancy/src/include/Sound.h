#pragma once
#include <xaudio2.h>
#include <x3daudio.h>

#include "NFObject.h"
#include "Utility.h"

namespace nf {
	struct Asset;
	class Entity;

	class Sound : public NFObject {
	public:
		Sound();

		void create(Asset* soundAsset);
		void setVolume(double volume);
		void setEntity(Entity& entity);
		void setPosition(const Vec3& position);
		void play(bool loop = false);
		void stop();

		void destroy() override;
		~Sound();
	private:
		bool m_constructed;
		unsigned int m_dataSize;
		float m_volume;
		bool m_usePos;
		WAVEFORMATEXTENSIBLE m_format;
		unsigned char* m_buffer;
		IXAudio2SourceVoice* m_currentVoice;
		Entity* m_targetEntity;
		Vec3 m_soundPos;
		X3DAUDIO_EMITTER m_emitter;
		X3DAUDIO_LISTENER m_listener;
	};
}