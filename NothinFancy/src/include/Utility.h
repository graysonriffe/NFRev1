#pragma once
#include <chrono>
#include <iostream>


namespace nf {
#ifdef _DEBUG
#define __FILENAME__ strrchr(__FILE__, '\\') + 1
#define DEBUGINIT std::chrono::steady_clock::time_point Debug::m_initTime = std::chrono::high_resolution_clock::now();
#define Log(x) Debug::LogImp(x) s
#define Error(x) Debug::ErrorImp(x,__FILENAME__, __LINE__);\
DebugBreak();

	class Debug {
	private:
		static std::chrono::steady_clock::time_point m_initTime;
	public:
		static void LogImp(char* in);
		static void LogImp(std::string& in);
		static void ErrorImp(char* in, char* filename, int line);
	};

#else
#define DEBUGINIT
#define Log(x)
#define Error(x)
#endif
}