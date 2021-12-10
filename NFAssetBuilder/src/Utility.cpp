#include "Utility.h"

#include<iostream>
#include<thread>
#include <fstream>
#include <sstream>

#include<Windows.h>
#include <compressapi.h>

static COMPRESSOR_HANDLE cHandle;

void Log(const char* in) {
	std::cout << "[NFAssetBuilder] Info: " << in << "\n";
}

void Log(const std::string& in) {
	Log(in.c_str());
}

[[noreturn]]
void Error(const std::string& in) {
	HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(cmd, FOREGROUND_RED);
	std::cout << "[NFAssetBuilder] Error: " + in + "\n";
	SetConsoleTextAttribute(cmd, 7);
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::exit(-1);
}

void Success(const std::string& in) {
	HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(cmd, FOREGROUND_GREEN);
	std::cout << "[NFAssetBuilder] Success: " + in + "\n";
	SetConsoleTextAttribute(cmd, 7);
}

void initCompressor() {
	CreateCompressor(COMPRESS_ALGORITHM_XPRESS_HUFF, NULL, &cHandle);
}

std::string readFile(const std::string& filename) {
	std::ifstream in;
	in.open(filename, std::ios::binary);
	if (!in)
		Error("File \"" + (std::string)filename + (std::string)"\" could not be opened!");
	std::stringstream ss;
	ss << in.rdbuf();
	std::string read(ss.str());
	return read;
}

void writePack(const std::string& filename, std::string& in) {
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if (!out)
		Error("Pack \"" + filename + (std::string)"\" could not be written!");

	in.insert(0, "NFASSETPACK");

	Log("Encrypting...");
	for (unsigned int i = 0; i < in.size(); i++)
		in[i] += 100;

	Log("Compressing...");
	size_t compSize;
	Compress(cHandle, &in[0], in.size(), NULL, 0, &compSize);
	char* buff = new char[compSize];
	Compress(cHandle, &in[0], in.size(), buff, compSize, &compSize);

	out.write(buff, compSize);
	delete[] buff;
	out.close();
}

std::string getNewLine(std::stringstream& stringstream) {
	std::string out;
	std::getline(stringstream, out);

	if (out[out.size() - 1] == '\r')
		out.erase(out.size() - 1);

	return out;
}