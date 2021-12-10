#pragma once
#include <string>

void Log(const char* in);
void Log(const std::string& in);

void Error(const std::string& in);
void Success(const std::string& in);

void initCompressor();

std::string readFile(const std::string& filename);
void writePack(const std::string& filename, std::string& in);

std::string getNewLine(std::stringstream& stringstream);

struct Vec2 {
	Vec2() : x(0.0f), y(0.0f) {}
	
	Vec2(float x1) : x(x1), y(x1) {}
	
	Vec2(float x1, float y1) : x(x1), y(y1) {}

	Vec2(double x1) : x((float)x1), y((float)x1) {}

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
	
	float x, y;
};

struct Vec3 {
	Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

	Vec3(float x1) : x(x1), y(x1), z(x1) {}

	Vec3(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {}

	Vec3(double x1) : x((float)x1), y((float)x1), z((float)x1) {}

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
		return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
	}

	float x, y, z;
};