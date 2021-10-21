#pragma once
#include <thread>
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
//Prints error message and breaks the debugger in debug mode
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
//Shows error dialog with specified message then exits
#define Error(x) {MessageBox(FindWindow(L"NFClass", NULL), toWide(x).data(), L"NF Engine Error", MB_OK | MB_ICONERROR);\
std::exit(-1);}
#endif
	struct Vec2 {
		Vec2() : x(0.0), y(0.0) {}
		Vec2(double x1) : x(x1), y(x1) {}
		Vec2(double x1, double y1) : x(x1), y(y1) {}
		Vec2 operator*(const double scalar) {
			return Vec2(x * scalar, y * scalar);
		}
		Vec2& operator+=(const Vec2& rhs) {
			this->x += rhs.x;
			this->y += rhs.y;
			return *this;
		}
		Vec2& operator-=(const Vec2& rhs) {
			this->x -= rhs.x;
			this->y -= rhs.y;
			return *this;
		}
		bool operator==(const Vec2& rhs) {
			return this->x == rhs.x && this->y == rhs.y;
		}
		double x, y;
	};
	struct Vec3 {
		Vec3() : x(0.0), y(0.0), z(0.0) {}
		Vec3(double x1) : x(x1), y(x1), z(x1) {}
		Vec3(double x1, double y1, double z1) : x(x1), y(y1), z(z1) {}
		Vec3 operator*(const double scalar) {
			return Vec3(x * scalar, y * scalar, z * scalar);
		}
		Vec3& operator+=(const Vec3& rhs) {
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			return *this;
		}
		Vec3& operator-=(const Vec3& rhs) {
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			return *this;
		}
		bool operator==(const Vec3& rhs) {
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z	;
		}
		double x, y, z;
	};

	const std::wstring toWide(const char* in);
	const std::wstring toWide(const std::string& in);
	void writeFile(const std::string& filename, const std::string& in, bool encrypted = false);
	std::string readFile(const std::string& filename, bool compressed = false);
}