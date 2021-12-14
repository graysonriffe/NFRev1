#include "nf/Utility.h"

#include <iostream>
#include <sstream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>
#include <compressapi.h>
#include "glm/glm.hpp"

#include "nf/Config.h"

namespace nf {
	static DECOMPRESSOR_HANDLE s_dHandle;

	static const float deg2rad = (float)M_PI / 180.0f;

#ifdef _DEBUG
	NFDEBUGINIT;

	void Debug::startTimer() {
		m_timerStarted = true;
		m_initTime = std::chrono::steady_clock::now();
	}

	void Debug::stopTimer() {
		m_timerStarted = false;
	}

	void Debug::LogImp(const char* in) {
		if(m_timerStarted)
			printCurrentTime();
		std::printf("NF Log: %s\n", in);
	}

	void Debug::LogImp(const std::string& in) {
		if (m_timerStarted)
			printCurrentTime();
		std::printf("NF Log: ");
		std::cout << in << "\n";
	}

	void Debug::LogImp(int in) {
		if (m_timerStarted)
			printCurrentTime();
		std::printf("NF Log: %i\n", in);
	}

	void Debug::LogImp(float in) {
		if (m_timerStarted)
			printCurrentTime();
		std::printf("NF Log: %.4f\n", in);
	}
	//TODO: Test every Error in release mode
	void Debug::ErrorImp(const char* in, const char* filename, int line) {
		if (m_timerStarted)
			printCurrentTime();
		static HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(cmd, FOREGROUND_RED);
		std::printf("NF Error (%s, %i): %s\n", filename, line, in);
		SetConsoleTextAttribute(cmd, 7);
	}

	void Debug::ErrorImp(const std::string& in, const char* filename, int line) {
		if (m_timerStarted)
			printCurrentTime();
		static HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(cmd, FOREGROUND_RED);
		std::printf("NF Error (%s, %i): ", filename, line);
		std::cout << in << "\n";
		SetConsoleTextAttribute(cmd, 7);
	}

	void Debug::printCurrentTime() {
		std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> dur = now - m_initTime;
		std::printf("[%.4f] ", dur.count());
	}

	Timer::Timer(const std::string& function, bool onEnter) {
		m_funcName = function;
		m_initTime = std::chrono::steady_clock::now();
		m_loading = onEnter;
		if (!m_loading)
			NFLog("Started timing \"" + m_funcName + (std::string)"\"");
	}

	Timer::~Timer() {
		std::chrono::duration<float> dur = std::chrono::steady_clock::now() - m_initTime;
		if (!m_loading)
			NFLog("\"" + m_funcName + (std::string)"\" took " + std::to_string(dur.count() * 1000.0f) + (std::string)" ms.");
		else
			NFLog("Loading took " + std::to_string(dur.count()) + (std::string)" seconds.");
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

		float cy = std::cosf(in.z * deg2rad / 2);
		float sy = std::sinf(in.z * deg2rad / 2);
		float cp = std::cosf(in.y * deg2rad / 2);
		float sp = std::sinf(in.y * deg2rad / 2);
		float cr = std::cosf(in.x * deg2rad / 2);
		float sr = std::sinf(in.x * deg2rad / 2);

		outW = cr * cp * cy + sr * sp * sy;
		outX = sr * cp * cy - cr * sp * sy;
		outY = cr * sp * cy + sr * cp * sy;
		outZ = cr * cp * sy - sr * sp * cy;

		return { outX, outY, outZ, outW };
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
			NFError("File \"" + (std::string)filename + (std::string)"\" could not be written!");
		std::string write(in);
		if (encrypted) {
			for (unsigned int i = 0; i < write.size(); i++)
				write[i] = write[i] + 100;
			write.insert(0, "NFEF");
		}
		out << write;
		out.close();
	}

	bool readFile(const std::string& filename, std::string& out, bool assetPack) {
		if (!s_dHandle)
			CreateDecompressor(COMPRESS_ALGORITHM_XPRESS_HUFF, NULL, &s_dHandle);

		std::ifstream in;
		in.open(filename, std::ios::binary);
		if (!in)
			return false;
		std::stringstream ss;
		ss << in.rdbuf();
		out = ss.str();

		if (assetPack) {
			size_t decompSize;
			Decompress(s_dHandle, &out[0], out.size(), NULL, 0, &decompSize);
			char* buff = new char[decompSize];
			Decompress(s_dHandle, &out[0], out.size(), buff, decompSize, &decompSize);
			out = std::string(buff, decompSize);
			delete[] buff;

			for (unsigned int i = 0; i < out.size(); i++)
				out[i] = out[i] - 100;
		}
		return true;
	}
}

//Nvidia Optimius support
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}