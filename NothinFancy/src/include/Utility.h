#pragma once
#include <chrono>
#include <string>
#include <Windows.h>

namespace nf {
#ifdef _DEBUG
//Strips __FILE__ down to only the name of the file
#define __FILENAME__ strrchr(__FILE__, '\\') + 1
//Initializes static variables needed for debugging
#define DEBUGINIT std::chrono::steady_clock::time_point Debug::m_initTime = std::chrono::high_resolution_clock::now();
//Sleep for an amount of seconds
#define SleepS(x) std::this_thread::sleep_for(std::chrono::seconds(x))
//Sleep for an amount of milliseconds
#define SleepMS(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
//Prints a nicely-formatted message complete with a timestamp
#define Log(x) nf::Debug::LogImp(x)
//Prints error message and breaks the debugger
#define Error(x) {nf::Debug::ErrorImp(x,__FILENAME__, __LINE__);\
__debugbreak();}

	class Debug {
	private:
		static std::chrono::steady_clock::time_point m_initTime;
		static std::chrono::duration<float> getCurrentTime();
	public:
		static void LogImp(const char* in);
		static void LogImp(const std::string& in);
		static void LogImp(int in);
		static void LogImp(double in);
		static void ErrorImp(const char* in, const char* filename, int line);
		static void ErrorImp(const std::string& in, const char* filename, int line);
	};
#else
#define DEBUGINIT
#define Log(x)
#define Error(x) {MessageBox(FindWindow(L"NFClass", NULL), toWide(x), L"NF Engine Error", MB_OK | MB_ICONERROR);\
std::exit(-1);}
#endif

	const wchar_t* toWide(const char* in);
	const wchar_t* toWide(const std::string& in);
	bool writeFile(const char* filename, const std::string& in);
	std::string readFile(const char* filename);
}