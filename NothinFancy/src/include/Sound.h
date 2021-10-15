#pragma once
#include "NFObject.h"
#include "AudioEngine.h"
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
		float m_volume;
		bool m_usePos;
		bool m_useEntity;
		WAVEFORMATEXTENSIBLE m_format;
		XAUDIO2_BUFFER m_xBuffer;
		unsigned char* m_buffer;
		Entity* m_targetEntity;
		Vec3 m_soundPos;
	};
}