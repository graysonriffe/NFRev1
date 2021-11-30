#pragma once
#include "nf/NFObject.h"
#include "nf/AudioEngine.h"
#include "nf/Utility.h"

namespace nf {
	struct Asset;
	class Entity;

	/**
	 * @brief A stream of audio; Could be a sound effect or music
	 * 
	 * NF currently supports WAV and Ogg Vorbis files.
	*/
	class Sound : public NFObject {
	public:
		/**
		 * @brief Constructor
		 * 
		 * See @ref obLifetime
		*/
		Sound();
		/**
		 * @brief Creates a sound
		 * @param soundAsset A sound asset retrieved from an AssetPack
		 * 
		 * The referenced sound can be of any format. This function will take care of
		 * figuring that out and loading it accordingly.
		*/
		void create(Asset* soundAsset);
		/**
		 * @brief Sets the volume of the sound
		 * @param volume Volume; Can be higher than 1.0f
		*/
		void setVolume(float volume);
		/**
		 * @brief Sets the target Entity of the sound
		 * @param entity The target entity
		 * 
		 * This function will bind the sound to a specific entity. When it is played,
		 * it will sound like the sound is coming from the target entity.
		*/
		void setEntity(Entity& entity);
		/**
		 * @brief Sets the static location the sound should play at
		 * @param position A position vector
		*/
		void setPosition(const Vec3& position);
		/**
		 * @brief Plays the sound
		 * @param loop If the sound should forever loop when it completes playing
		 * 
		 * This function can be called multiple times to play multiple instances of the
		 * sound.
		*/
		void play(bool loop = false);
		/**
		 * @brief Stops the sound
		 * 
		 * This function will stop all instances of the sound if multiple are playing
		*/
		void stop();

		void destroy() override;
		~Sound();
	private:
		size_t loadOGG(std::string& data);
		size_t loadWAV(std::string& data);

		bool m_constructed;
		float m_volume;
		bool m_usePos;
		bool m_useEntity;
		WAVEFORMATEXTENSIBLE m_format;
		XAUDIO2_BUFFER m_xBuffer;
		char* m_buffer;
		Entity* m_targetEntity;
		Vec3 m_soundPos;
	};
}