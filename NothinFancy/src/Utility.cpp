#include "Utility.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Config.h"

namespace nf {
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

	void Debug::ErrorImp(const char* in, const char* filename, int line) {
		std::chrono::duration<float> time = getCurrentTime();
		HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(cmd, FOREGROUND_RED);
		std::printf("[%.4f] Error (%s, %i): %s\n", time.count(), filename, line, in);
		SetConsoleTextAttribute(cmd, 7);
		CloseHandle(cmd);
	}

	void Debug::ErrorImp(const std::string& in, const char* filename, int line) {
		std::chrono::duration<float> time = getCurrentTime();
		HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(cmd, FOREGROUND_RED);
		std::printf("[%.4f] Error (%s, %i): ", time.count(), filename, line);
		std::cout << in << "\n";
		SetConsoleTextAttribute(cmd, 7);
		CloseHandle(cmd);
	}

	std::chrono::duration<float> Debug::getCurrentTime() {
		std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
		return now - m_initTime;
	}
#endif

	Win32Res::Win32Res(int id) :
		ptr(nullptr),
		size(0)
	{
		HRSRC src = FindResource(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
		if (src) {
			HGLOBAL temp = LoadResource(NULL, src);
			if (temp) {
				ptr = LockResource(temp);
				size = (size_t)SizeofResource(NULL, src);
			}
		}
	}

	const wchar_t* toWide(const char* in) {
		int length = std::strlen(in) + 1;
		wchar_t* out = new wchar_t[length];
		MultiByteToWideChar(CP_ACP, NULL, in, -1, out, length);
		return out;
	}
	const wchar_t* toWide(const std::string& in) {
		const char* cstr = in.c_str();
		int length = std::strlen(cstr) + 1;
		wchar_t* out = new wchar_t[length];
		MultiByteToWideChar(CP_ACP, NULL, cstr, -1, out, length);
		return out;
	}

	bool writeFile(const char* filename, const std::string& in, bool encrypted) {
		std::string file(filename);
		if (file.find('/') != std::string::npos || file.find('\\') != std::string::npos) {
			int pos = file.find_last_of("/\\");
			std::string temp = file.substr(0, pos);
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
		out.open(filename);
		if (!out)
			Error("File \"" + (std::string)filename + (std::string)"\" could not be written!");
		std::string write(in);
		if (encrypted) {
			for (unsigned int i = 0; i < write.size(); i++)
				write[i] = write[i] + 100;
		}
		out << write;
		out.close();
		return true;
	}

	std::string readFile(const char* filename, bool encrypted) {
		std::ifstream in;
		in.open(filename);
		if (!in)
			Error("File \"" + (std::string)filename + (std::string)"\" could not be read!");
		std::stringstream ss;
		ss << in.rdbuf();
		std::string read(ss.str());
		if (encrypted) {
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