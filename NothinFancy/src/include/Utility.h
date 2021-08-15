#pragma once
#include <chrono>
#include <thread>
#include <iostream>
#include <string>

namespace nf {
#ifdef _DEBUG
#define __FILENAME__ strrchr(__FILE__, '\\') + 1
#define DEBUGINIT std::chrono::steady_clock::time_point Debug::m_initTime = std::chrono::high_resolution_clock::now();
#define SleepS(x) std::this_thread::sleep_for(std::chrono::seconds(x))
#define SleepMS(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
#define Log(x) Debug::LogImp(x)
#define Error(x) Debug::ErrorImp(x,__FILENAME__, __LINE__);\
DebugBreak();

	class Debug {
	private:
		static std::chrono::steady_clock::time_point m_initTime;
		static std::chrono::duration<float> getCurrentTime();
	public:
		static void LogImp(const char* in);
		static void LogImp(const std::string& in);
		static void ErrorImp(const char* in, const char* filename, int line);
	};
#else
#define DEBUGINIT
#define Log(x)
#define Error(x)
#endif

	const wchar_t* toWide(const char* in);
}