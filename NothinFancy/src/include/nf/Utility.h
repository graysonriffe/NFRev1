#pragma once
#include <thread>
#include <chrono>
#include <string>
#include <Windows.h>

/**
 * @brief Nothin' Fancy namespace
 * 
 * Every class and struct lives inside of this namespace.
 * 
 * It could be useful to `using` this namespace:
 * 
 * ~~~
 * using namespace nf;
 * ~~~
*/
namespace nf {
#if defined(_DEBUG) || defined(doxygen)
//Strips __FILE__ down to only the name of the file
#define __FILENAME__ strrchr(__FILE__, '\\') + 1
//Initializes static variables needed for debugging
#define NFDEBUGINIT std::chrono::steady_clock::time_point Debug::m_initTime = std::chrono::high_resolution_clock::now(); \
bool Debug::m_timerStarted = false
/**
* @defgroup macros Macros
* 
* Macros to aid in debugging and developing with NF
* 
* @{
*/
/**
* Pauses the engine for a set amount of seconds
*/
#define NFSleepS(x) std::this_thread::sleep_for(std::chrono::seconds(x))
/**
* Pauses the engine for a set amount of milliseconds
*/
#define NFSleepMS(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
/**
* In debug mode, this prints a nicely-formatted message complete with a timestamp.
* 
* In release mode, this does nothing.
*/
#define NFLog(x) nf::Debug::LogImp(x)
/**
* In debug mode, the error message is printed to the console and the debugger is broken.
* 
* In release mode, the error message is shown in a message box and the application
* closes when the message box is closed.
*/
#define NFError(x) {nf::Debug::ErrorImp(x,__FILENAME__, __LINE__);\
__debugbreak();}
/**
* A timer useful for timing functions
* 
* To time a function, place this macro at the beginning of it:
* 
* ~~~
* void myFunc() {
*	NFTimer;
*	//Rest of function to be timed...
* } //Prints here
* ~~~
* 
* The result will be logged when the scope it's declared in ends.
*/
#define NFTimer nf::Timer _nfTimer(__func__)
/**
* @}
*/
#ifndef NFIMPL
#define NFTimerLoad nf::Timer _nfTimer(__func__, true);
#endif

	class Debug {
	public:
		static void startTimer();
		static void stopTimer();

		static void LogImp(const char* in);
		static void LogImp(const std::string& in);
		static void LogImp(int in);
		static void LogImp(float in);
		[[noreturn]]
		static void ErrorImp(const char* in, const char* filename, int line);
		[[noreturn]]
		static void ErrorImp(const std::string& in, const char* filename, int line);
	private:
		static void printCurrentTime();
		static std::chrono::steady_clock::time_point m_initTime;
		static bool m_timerStarted;
	};

	class Timer {
	public:
		Timer(const std::string& function, bool onEnter = false);

		~Timer();
	private:
		std::chrono::steady_clock::time_point m_initTime;
		bool m_loading;
		std::string m_funcName;
	};
#else
#define NFDEBUGINIT
#define NFSleepS(x)
#define NFSleepMS(x)
#define NFLog(x)
#define NFError(x) {MessageBox(FindWindow(L"NFClass", NULL), toWide(x).data(), L"NF Engine Error", MB_OK | MB_ICONERROR);\
std::exit(-1);}
#define NFTimer
#define NFTimerLoad
#endif

	/**
	 * @brief Two component vector
	 * 
	 * Similar to Vec3 and Vec4.
	 * 
	 * Supports operators + - * / += -= *= /= ==
	*/
	struct Vec2 {
		/**
		 * @brief Default constructor
		 * 
		 * Initializes all components to 0.0f
		*/
		Vec2() : x(0.0f), y(0.0f) {}
		/**
		 * @brief Single constructor
		 * @param x1 
		 * 
		 * Sets all components to x1
		*/
		Vec2(float x1) : x(x1), y(x1) {}
		/**
		 * @brief Double constructor
		 * @param x1 
		 * @param y1 
		 * 
		 * Initializes the vector with the specified values
		*/
		Vec2(float x1, float y1) : x(x1), y(y1) {}
		/**
		 * @brief Single constructor (double version)
		 * @param x1 
		 * 
		 * Double compatibility
		*/
		Vec2(double x1) : x((float)x1), y((float)x1) {}
		/**
		 * @brief Double constructor (double version)
		 * @param x1 
		 * @param y1 
		 * 
		 * Double compatibility
		*/
		Vec2(double x1, double y1) : x((float)x1), y((float)y1) {}
		Vec2 operator+(const Vec2& rhs) const {
			return Vec2(x + rhs.x, y + rhs.y);
		}
		Vec2 operator-(const Vec2& rhs) const {
			return Vec2(x - rhs.x, y - rhs.y);
		}
		Vec2 operator*(const float scalar) const {
			return Vec2(x * scalar, y * scalar);
		}
		Vec2 operator/(const Vec2& rhs) const {
			return Vec2(x / rhs.x, y / rhs.y);
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
		Vec2& operator*=(const Vec2& rhs) {
			this->x *= rhs.x;
			this->y *= rhs.y;
			return *this;
		}
		Vec2& operator/=(const Vec2& rhs) {
			this->x /= rhs.x;
			this->y /= rhs.y;
			return *this;
		}
		bool operator==(const Vec2& rhs) {
			return this->x == rhs.x && this->y == rhs.y;
		}
		/**
		 * @brief X component
		*/
		float x;
		/**
		 * @brief Y components
		*/
		float y;
	};
	/**
	 * @brief Three component vector
	 *
	 * Similar to Vec2 and Vec4
	 * 
	 * Supports operators + - * / += -= *= /= ==
	*/
	struct Vec3 {
		/**
		 * @brief Default constructor
		 *
		 * Initializes all components to 0.0f
		*/
		Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
		/**
		 * @brief Single constructor
		 * @param x1
		 *
		 * Sets all components to x1
		*/
		Vec3(float x1) : x(x1), y(x1), z(x1) {}
		/**
		 * @brief Triple constructor
		 * @param x1
		 * @param y1
		 * @param z1
		 *
		 * Initializes the vector with the specified values
		*/
		Vec3(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {}
		/**
		 * @brief Single constructor (double version)
		 * @param x1
		 *
		 * Double compatibility
		*/
		Vec3(double x1) : x((float)x1), y((float)x1), z((float)x1) {}
		/**
		 * @brief Triple constructor (double version)
		 * @param x1
		 * @param y1
		 * @param z1
		 *
		 * Double compatibility
		*/
		Vec3(double x1, double y1, double z1) : x((float)x1), y((float)y1), z((float)z1) {}
		Vec3 operator+(const Vec3& rhs) const {
			return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
		}
		Vec3 operator-(const Vec3& rhs) const {
			return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
		}
		Vec3 operator*(const float scalar) const {
			return Vec3(x * scalar, y * scalar, z * scalar);
		}
		Vec3 operator/(const Vec3& rhs) const {
			return Vec3(x / rhs.x, y / rhs.y, z / rhs.z);
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
		Vec3& operator*=(const Vec3& rhs) {
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
			return *this;
		}
		Vec3& operator/=(const Vec3& rhs) {
			this->x /= rhs.x;
			this->y /= rhs.y;
			this->z /= rhs.z;
			return *this;
		}
		bool operator==(const Vec3& rhs) {
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z	;
		}
		/**
		 * @brief X component
		*/
		float x;
		/**
		 * @brief Y component
		*/
		float y;
		/**
		 * @brief Z component
		*/
		float z;
	};
	/**
	 * @brief Four component vector
	 *
	 * Similar to Vec2 and Vec3
	 * 
	 * Supports operators + - * / += -= *= /= ==
	*/
	struct Vec4 {
		/**
		 * @brief Default constructor
		 *
		 * Initializes all components to 0.0f
		*/
		Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		/**
		 * @brief Single constructor
		 * @param x1
		 *
		 * Sets all components to x1
		*/
		Vec4(float x1) : x(x1), y(x1), z(x1), w(x1) {}
		/**
		 * @brief Quadruple constructor
		 * @param x1
		 * @param y1
		 * @param z1
		 * @param w1
		 *
		 * Initializes the vector with the specified values
		*/
		Vec4(float x1, float y1, float z1, float w1) : x(x1), y(y1), z(z1), w(w1) {}
		/**
		 * @brief Single constructor (double version)
		 * @param x1
		 *
		 * Double compatibility
		*/
		Vec4(double x1) : x((float)x1), y((float)x1), z((float)x1), w((float)x1) {}
		/**
		 * @brief Quadruple constructor (double version)
		 * @param x1
		 * @param y1
		 * @param z1
		 * @param w1
		 *
		 * Double compatibility
		*/
		Vec4(double x1, double y1, double z1, double w1) : x((float)x1), y((float)y1), z((float)z1), w((float)w1) {}
		Vec4 operator+(const Vec4& rhs) const {
			return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}
		Vec4 operator-(const Vec4& rhs) const {
			return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}
		Vec4 operator*(const float scalar) const {
			return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
		}
		Vec4 operator/(const Vec4& rhs) const {
			return Vec4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
		}
		Vec4& operator+=(const Vec4& rhs) {
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			this->w += rhs.w;
			return *this;
		}
		Vec4& operator-=(const Vec4& rhs) {
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			this->w -= rhs.w;
			return *this;
		}
		Vec4& operator*=(const Vec4& rhs) {
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
			this->w *= rhs.w;
			return *this;
		}
		Vec4& operator/=(const Vec4& rhs) {
			this->x /= rhs.x;
			this->y /= rhs.y;
			this->z /= rhs.z;
			this->w /= rhs.w;
			return *this;
		}
		bool operator==(const Vec4& rhs) {
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w;
		}
		/**
		 * @brief X component
		*/
		float x;
		/**
		 * @brief Y component
		*/
		float y;
		/**
		 * @brief Z component
		*/
		float z;
		/**
		 * @brief W component
		*/
		float w;
	};

#ifndef NFIMPL
	const std::wstring toWide(const char* in);
	const std::wstring toWide(const std::string& in);
	Vec4 degToQuat(const Vec3& in);
#endif

	/**
	 * @brief Writes a stream of bytes as as std::string into a file in a specified location
	 * @param filename Path and name of file to be written, including extensions; Relative or absolute
	 * @param in Input std::string to be written
	 * @param encrypted Write the file encrypted?
	 * 
	 * This function, as well as readFile, supports a basic and unsecure form of encryption.
	 * This is meant to discourage players from easily modifying stats, saves, etc.
	*/
	void writeFile(const std::string& filename, const std::string& in, bool encrypted = false);
	/**
	 * @brief Reads a file's bytes into an std::string
	 * @param filename Path and name of file to be read, including extensions; Relative or absolute
	 * @param compressed Internal use only as of now
	 * @return An std::string containing the specified file's bytes
	 * 
	 * This function automatically detects whether or not the target file is encrypted
	 * and decrypts it if it is.
	 * 
	 * @todo If files aren't found, the engine errors. Change this.
	*/
	std::string readFile(const std::string& filename, bool compressed = false);
}