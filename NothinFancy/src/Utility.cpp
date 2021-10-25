#include "Utility.h"
#include <iostream>
#include <sstream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>
#include <compressapi.h>
#include "glm/glm.hpp"

#include "Config.h"

namespace nf {
	static DECOMPRESSOR_HANDLE s_dHandle;

	static const double deg2rad = M_PI / 180.0f;

#ifdef _DEBUG
	void Debug::LogImp(const char* in) {
		std::chrono::duration<float> time = getCurrentTime();
		std::printf("[%.4f] Debug: %s\n", time.count(), in);
	}

	void Debug::LogImp(const std::string& in) {
		std::chrono::duration<float> time = getCurrentTime();
		std::printf("[%.4f] Debug: ", time.count());
		std::cout << in << "\n";
	}

	void Debug::LogImp(int in) {
		std::chrono::duration<float> time = getCurrentTime();
		std::printf("[%.4f] Debug: %i\n", time.count(), in);
	}

	void Debug::LogImp(double in) {
		std::chrono::duration<float> time = getCurrentTime();
		std::printf("[%.4f] Debug: %.4f\n", time.count(), in);
	}
	//TODO: Test every Error in release mode
	void Debug::ErrorImp(const char* in, const char* filename, int line) {
		std::chrono::duration<float> time = getCurrentTime();
		static HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(cmd, FOREGROUND_RED);
		std::printf("[%.4f] Error (%s, %i): %s\n", time.count(), filename, line, in);
		SetConsoleTextAttribute(cmd, 7);
	}

	void Debug::ErrorImp(const std::string& in, const char* filename, int line) {
		std::chrono::duration<float> time = getCurrentTime();
		static HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(cmd, FOREGROUND_RED);
		std::printf("[%.4f] Error (%s, %i): ", time.count(), filename, line);
		std::cout << in << "\n";
		SetConsoleTextAttribute(cmd, 7);
	}

	std::chrono::duration<float> Debug::getCurrentTime() {
		std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
		return now - m_initTime;
	}
#endif

	const std::wstring toWide(const char* in) {
		unsigned int length = (unsigned int)std::strlen(in) + 1;
		wchar_t* out = new wchar_t[length];
		MultiByteToWideChar(CP_ACP, NULL, in, -1, out, length);
		std::wstring str(out, length);

		delete[] out;
		return str;
	}
	const std::wstring toWide(const std::string& in) {
		const char* cstr = in.c_str();
		unsigned int length = (unsigned int)std::strlen(cstr) + 1;
		wchar_t* out = new wchar_t[length];
		MultiByteToWideChar(CP_ACP, NULL, cstr, -1, out, length);
		std::wstring str(out, length);

		delete[] out;
		return str;
	}

	Vec4 degToQuat(const Vec3& in) {
		float outX, outY, outZ, outW;

		float cy = (float)std::cos(in.z * deg2rad * 0.5);
		float sy = (float)std::sin(in.z * deg2rad * 0.5);
		float cp = (float)std::cos(in.y * deg2rad * 0.5);
		float sp = (float)std::sin(in.y * deg2rad * 0.5);
		float cr = (float)std::cos(in.x * deg2rad * 0.5);
		float sr = (float)std::sin(in.x * deg2rad * 0.5);

		outW = cr * cp * cy + sr * sp * sy;
		outX = sr * cp * cy - cr * sp * sy;
		outY = cr * sp * cy + sr * cp * sy;
		outZ = cr * cp * sy - sr * sp * cy;

		return {outX, outY, outZ, outW};
	}

	void writeFile(const std::string& filename, const std::string& in, bool encrypted) {
		if (filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
			size_t pos = filename.find_last_of("/\\");
			std::string temp = filename.substr(0, pos);
			std::wstring folders(temp.begin(), temp.end());
			WCHAR exe[MAX_PATH];
			GetModuleFileName(GetModuleHandle(NULL), exe, MAX_PATH);
			std::wstring rootDir(exe);
			pos = rootDir.find_last_of(L"/\\");
			rootDir = rootDir.substr(0, pos + 1);
			folders = rootDir + folders;
			CreateDirectory(folders.c_str(), NULL);
		}
		std::ofstream out;
		out.open(filename, std::ios::binary);
		if (!out)
			Error("File \"" + (std::string)filename + (std::string)"\" could not be written!");
		std::string write(in);
		if (encrypted) {
			for (unsigned int i = 0; i < write.size(); i++)
				write[i] = write[i] + 100;
			write.insert(0, "NFEF");
		}
		out << write;
		out.close();
	}

	std::string readFile(const std::string& filename, bool compressed) {
		if (!s_dHandle)
			CreateDecompressor(COMPRESS_ALGORITHM_XPRESS_HUFF, NULL, &s_dHandle);

		std::ifstream in;
		in.open(filename, std::ios::binary);
		if (!in)
			Error("File \"" + (std::string)filename + (std::string)"\" could not be read!");
		std::stringstream ss;
		ss << in.rdbuf();
		std::string read(ss.str());

		if (compressed) {
			size_t decompSize;
			Decompress(s_dHandle, &read[0], read.size(), NULL, 0, &decompSize);
			char* buff = new char[decompSize];
			Decompress(s_dHandle, &read[0], read.size(), buff, decompSize, &decompSize);
			read = std::string(buff, decompSize);
			delete[] buff;
		}

		if (read.size() > 4 && read.substr(0, 4) == "NFEF") {
			read = read.substr(4);
			for (unsigned int i = 0; i < read.size(); i++)
				read[i] = read[i] - 100;
		}
		return read;
	}
}

//Nvidia Optimius support
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}