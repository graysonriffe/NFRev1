#include "Sound.h"

#include "vorbis/vorbisfile.h"

#include "Application.h"
#include "Assets.h"
#include "Entity.h"

namespace nf {
#pragma region Ogg File Loading
	struct oggFileMem {
		char* data;
		int size;
		size_t pointer;
	};

	size_t v_read(void* out, size_t sizeofByte, size_t toRead, void* in) {
		oggFileMem* file = (oggFileMem*)in;
		if (file == nullptr) return -1;

		size_t actual, left = file->size - file->pointer;
		if ((toRead * sizeofByte) < left)
			actual = toRead * sizeofByte;
		else
			actual = left;

		if (actual) {
			std::memcpy(out, (char*)file->data + file->pointer, actual);
			file->pointer += actual;
		}

		return actual;
	}

	int v_seek(void* in, ogg_int64_t offset, int origin) {
		oggFileMem* file = (oggFileMem*)in;
		if (file == nullptr) return -1;

		switch (origin) {
			case SEEK_SET: {
				ogg_int64_t actual;
				if (file->size >= offset)
					actual = offset;
				else
					actual = file->size;

				file->pointer = actual;
				break;
			}
			case SEEK_CUR: {
				int left = file->size - (int)file->pointer;

				ogg_int64_t actual;
				if (offset < left)
					actual = offset;
				else
					actual = left;

				file->pointer += actual;
				break;
			}
			case SEEK_END: {
				file->pointer = file->size + 1;
				break;
			}
		}
		return 0;
	}

	int v_close(void* in) {
		return 0;
	}

	long v_tell(void* in) {
		oggFileMem* file = (oggFileMem*)in;
		if (file == nullptr) return -1;

		return (long)file->pointer;
	}
#pragma endregion

	Sound::Sound() :
		m_constructed(false),
		m_volume(1.0f),
		m_usePos(false),
		m_useEntity(false),
		m_format({ 0 }),
		m_xBuffer({ 0 }),
		m_buffer(nullptr),
		m_targetEntity(nullptr)
	{

	}

	void Sound::create(Asset* soundAsset) {
		if (m_constructed)
			NFError("Sound already created!");
		m_constructed = true;
		ASound* sound;
		if ((sound = dynamic_cast<ASound*>(soundAsset)) == nullptr)
			NFError("Non-sound asset passed to Sound::create!");
		std::string data(sound->data, sound->size);

		size_t dataSize;
		if (data.find("OggS") == 0)
			dataSize = loadOGG(data);
		else if (data.find("RIFF") == 0)
			dataSize = loadWAV(data);
		else
			NFError("Sound asset not of correct format!");

		m_xBuffer.pAudioData = (unsigned char*)m_buffer;
		m_xBuffer.AudioBytes = (unsigned int)dataSize;
		m_xBuffer.Flags = XAUDIO2_END_OF_STREAM;

		if (!Application::getApp()->getCurrentState()->isRunning())
			Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
	}

	size_t Sound::loadOGG(std::string& data) {
		m_format.Format.cbSize = sizeof(m_format.Format);
		size_t pos = data.find("vorbis") + 10;
		m_format.Format.nChannels = data[pos];
		m_format.Format.wBitsPerSample = 16;
		pos++;
		m_format.Format.nSamplesPerSec = *(unsigned int*)&data[pos];
		m_format.Format.nAvgBytesPerSec = m_format.Format.nSamplesPerSec * m_format.Format.nChannels * 2;
		m_format.Format.nBlockAlign = m_format.Format.nChannels * 2;
		m_format.Format.wFormatTag = 1;

		OggVorbis_File file;
		oggFileMem memFile;
		memFile.pointer = 0;
		memFile.size = (int)data.size();
		memFile.data = &data[0];

		ov_callbacks callbacks;
		callbacks.read_func = v_read;
		callbacks.seek_func = v_seek;
		callbacks.close_func = v_close;
		callbacks.tell_func = v_tell;

		int open = ov_open_callbacks(&memFile, &file, nullptr, 0, callbacks);
		char* buff = new char[65536 * 1000];

		int stream = 0;
		int read = 1;
		size_t size = 0;

		while (read) {
			read = ov_read(&file, buff + size, 4096, 0, 2, 1, &stream);
			size += read;
		}

		ov_clear(&file);

		m_buffer = new char[size];
		for (unsigned int i = 0; i < size; i++)
			m_buffer[i] = buff[i];

		delete[] buff;

		return size;
	}

	size_t Sound::loadWAV(std::string& data) {
		unsigned int fileSize = *(unsigned int*)&data[4];
		size_t fmtPos;
		if ((fmtPos = data.find("fmt")) == std::string::npos)
			NFError("WAV not of correct format!");
		std::memcpy(&m_format, &data[fmtPos + 8], 16);
		size_t dataPos;
		if ((dataPos = data.find("data")) == std::string::npos)
			NFError("WAV not of correct m_format!");
		unsigned int dataSize = *(unsigned int*)&data[dataPos + 4];
		m_buffer = new char[dataSize];
		std::memcpy(m_buffer, &data[dataPos + 8], dataSize);

		return dataSize;
	}

	void Sound::setVolume(float volume) {
		m_volume = volume;
	}

	void Sound::setEntity(Entity& entity) {
		m_targetEntity = &entity;
		m_useEntity = true;
		m_usePos = false;
	}

	void Sound::setPosition(const Vec3& position) {
		m_soundPos = position;
		m_usePos = true;
		m_useEntity = false;
	}

	void Sound::play(bool loop) {
		if (!Application::getApp()->getAudioEngine()->isActive()) return;

		if (loop)
			m_xBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;

		SoundData sd = { &m_format, &m_xBuffer, nullptr, true, m_volume, m_useEntity, m_targetEntity, m_usePos, m_soundPos };
		Application::getApp()->getAudioEngine()->addSound(sd);
	}

	void Sound::stop() {
		Application::getApp()->getAudioEngine()->stopSound(&m_xBuffer);
	}

	void Sound::destroy() {
		if (m_constructed) {
			delete m_buffer;
			m_buffer = nullptr;
		}
		m_constructed = false;
		m_volume = 1.0f;
		m_format = { 0 };
	}

	Sound::~Sound() {
		destroy();
	}
}