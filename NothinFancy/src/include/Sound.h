#pragma once
#include <xaudio2.h>

#include "NFObject.h"

namespace nf {
	struct Asset;

	class Sound : public NFObject {
	public:
		Sound();

		void create(Asset* soundAsset);
		void setVolume(double volume);
		void play(bool loop = false);
		void stop();

		void destroy() override;
		~Sound();
	private:
		bool m_constructed;
		unsigned int m_dataSize;
		float m_volume;
		WAVEFORMATEXTENSIBLE m_format;
		unsigned char* m_buffer;
		IXAudio2SourceVoice* m_currentVoice;
	};
}