#include "Utility.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "glm/glm.hpp"

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

	void writeFile(const std::string& filename, const std::string& in, bool encrypted) {
		if (filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
			int pos = filename.find_last_of("/\\");
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

	std::string readFile(const std::string& filename) {
		std::ifstream in;
		in.open(filename, std::ios::binary);
		if (!in)
			Error("File \"" + (std::string)filename + (std::string)"\" could not be read!");
		std::stringstream ss;
		ss << in.rdbuf();
		std::string read(ss.str());
		if (read.size() > 4 && read.substr(0, 4) == "NFEF") {
			read = read.substr(4);
			for (unsigned int i = 0; i < read.size(); i++)
				read[i] = read[i] - 100;
		}
		return read;
	}

	void parseOBJ(std::string& in, std::vector<float>& vbOut, std::vector<unsigned int>& ibOut, size_t& ibCountOut, std::vector<float>& tcOut, std::vector<float>& vnOut) {
		std::string file = in;
		std::vector<float> vbRaw, tcRaw, vnRaw;
		std::vector<unsigned int> vertexIndices, uvIndices, vnIndices;
		std::vector<float> tempVertices, tempTC, tempVN;

		bool tcPresent = false, vnPresent = false;
		while (true) {
			char line[500];
			int remove = 0;
			int result = sscanf_s(file.c_str(), "\n%s", line, (unsigned)_countof(line));
			if (result == EOF)
				break;
			if (std::strcmp(line, "v") == 0) {
				float x = 0.0, y = 0.0, z = 0.0;
				sscanf_s(file.c_str(), "\nv %f %f %f\n", &x, &y, &z);
				remove = 28;
				tempVertices.push_back(x);
				tempVertices.push_back(y);
				tempVertices.push_back(z);
			}
			else if (std::strcmp(line, "vt") == 0) {
				tcPresent = true;
				float u = 0.0, v = 0.0;
				sscanf_s(file.c_str(), "\nvt %f %f\n", &u, &v);
				remove = 18;
				tempTC.push_back(u);
				tempTC.push_back(v);
			}
			if (std::strcmp(line, "vn") == 0) {
				vnPresent = true;
				float x = 0.0, y = 0.0, z = 0.0;
				sscanf_s(file.c_str(), "\nvn %f %f %f\n", &x, &y, &z);
				remove = 20;
				tempVN.push_back(x);
				tempVN.push_back(y);
				tempVN.push_back(z);
			}
			else if (std::strcmp(line, "f") == 0) {
				unsigned int vertexIndex[3], uvIndex[3], vnIndex[3];
				sscanf_s(file.c_str(), "\nf %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &vnIndex[0], &vertexIndex[1], &uvIndex[1], &vnIndex[1], &vertexIndex[2], &uvIndex[2], &vnIndex[2]);
				remove = 15;
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				vnIndices.push_back(vnIndex[0]);
				vnIndices.push_back(vnIndex[1]);
				vnIndices.push_back(vnIndex[2]);
			}

			unsigned int pos = file.find(line) + strlen(line) + remove;
			file = file.substr(pos);
		}

		if (!tcPresent)
			Error("No texture coordinates found in model!");
		if (!vnPresent)
			Error("No normals found in model!");

		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];
			unsigned int vnIndex = vnIndices[i];
			float vertexX = tempVertices[(vertexIndex - 1) * 3];
			float vertexY = tempVertices[(vertexIndex - 1) * 3 + 1];
			float vertexZ = tempVertices[(vertexIndex - 1) * 3 + 2];
			float vertexU = tempTC[(uvIndex - 1) * 2];
			float vertexV = tempTC[(uvIndex - 1) * 2 + 1];
			float vnX = tempVN[(vnIndex - 1) * 3];
			float vnY = tempVN[(vnIndex - 1) * 3 + 1];
			float vnZ = tempVN[(vnIndex - 1) * 3 + 2];
			vbRaw.push_back(vertexX);
			vbRaw.push_back(vertexY);
			vbRaw.push_back(vertexZ);
			tcRaw.push_back(vertexU);
			tcRaw.push_back(vertexV);
			vnRaw.push_back(vnX);
			vnRaw.push_back(vnY);
			vnRaw.push_back(vnZ);
		}

		struct Vertex {
			float x;
			float y;
			float z;

			float u;
			float v;

			float vnX;
			float vnY;
			float vnZ;

			bool operator<(const Vertex other) const {
				return std::memcmp((void*)this, (void*)&other, sizeof(Vertex)) > 0;
			}
		};
		std::map<Vertex, unsigned int> vertexMap;
		for (unsigned int i = 0; i * 3 < vbRaw.size(); i++) {
			Vertex curr = { vbRaw[(i * 3)], vbRaw[(i * 3) + 1], vbRaw[(i * 3) + 2], tcRaw[(i * 2)], tcRaw[(i * 2) + 1], vnRaw[(i * 3)], vnRaw[(i * 3) + 1], vnRaw[(i * 3) + 2] };
			bool found = false;
			found = vertexMap.find(curr) != vertexMap.end();
			if (found) {
				unsigned int index = vertexMap[curr];
				ibOut.push_back(index);
				ibCountOut++;
			}
			else {
				vbOut.push_back(curr.x);
				vbOut.push_back(curr.y);
				vbOut.push_back(curr.z);
				tcOut.push_back(curr.u);
				tcOut.push_back(curr.v);
				vnOut.push_back(curr.vnX);
				vnOut.push_back(curr.vnY);
				vnOut.push_back(curr.vnZ);
				unsigned int index = (vbOut.size() / 3) - 1;
				ibOut.push_back(index);
				vertexMap[curr] = index;
				ibCountOut++;
			}
		}
	}
}

//Nvidia Optimius support
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}