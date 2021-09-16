#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <set>
#include <Windows.h>

void Log(const std::string& in) {
	std::cout << "[NFPackCreator] Info: " << in << "\n";
}

void Log(const char* in) {
	std::cout << "[NFPackCreator] Info: " << in << "\n";
}

__declspec(noreturn) void Error(const std::string& in) {
	HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(cmd, FOREGROUND_RED);
	std::cout << "[NFPackCreator] Error: " + in + "\n";
	SetConsoleTextAttribute(cmd, 7);
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::exit(-1);
}

void Success(const std::string& in) {
	HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(cmd, FOREGROUND_GREEN);
	std::cout << "[NFPackCreator] Success: " + in + "\n";
	SetConsoleTextAttribute(cmd, 7);
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

void writeFile(const std::string& filename, const std::string& in, bool encrypted) {
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if (!out)
		Error("File \"" + (std::string)filename + (std::string)"\" could not be written!");
	std::string write(in);
	if (encrypted) {
		for (unsigned int i = 0; i < write.size(); i++) {
			char temp = write[i] + 100;
			write[i] = temp;
		}
		write.insert(0, "NFEF");
	}
	out << write;
	out.close();
}

void getNeededImages(std::string mtl, std::set<std::string>& set) {
	while (mtl.size()) {
		unsigned int pos = mtl.find("map_");
		if (pos == std::string::npos)
			break;
		if (pos == mtl.find("map_Kd") || pos == mtl.find("map_Ks"))
			mtl = mtl.substr(pos + 7);
		else
			mtl = mtl.substr(pos + 9);
		std::stringstream ss(mtl);
		std::string temp;
		ss >> temp;
		unsigned int pos2 = temp.find_last_of("/\\");
		if (pos2 != std::string::npos)
			temp = temp.substr(pos2 + 1);
		set.insert(temp);
	}
}

int main(int argc, char* argv[]) {
	Log("Starting up");

	if (argc > 1) {
		if ((std::string)argv[1] == "-h") {
			Log("Nothin' Fancy Asset Pack Creator\nThis tool creates .nfpack files for the NF engine to load at runtime.\nA pack gets created for each directory in the working directory if there are only compatible files inside.\nSubdirectories are not ignored.");
			return 0;
		}
	}

	std::set<std::string> extensions;
	extensions.insert({ "shader", "obj", "png", "jpg", "ttf", "wav" });

	unsigned int dirCount = 0;
	const std::filesystem::path workingDir = std::filesystem::current_path();
	for (const auto& currDir : std::filesystem::directory_iterator(std::filesystem::current_path())) {
		if (!currDir.is_directory())
			continue;
		std::string filename = currDir.path().filename().string().append(".nfpack");
		/*if (filename == "base.nfpack")
			Error("Cannot create a pack called base.nfpack!");*/
		Log("Creating pack \"" + filename + (std::string)"\"");
		std::vector<std::string> packFilenames;
		std::string currFileExtension;
		std::string currFileContents;
		std::stringstream out;
		unsigned int fileCount = 0;
		for (const auto& curr : std::filesystem::recursive_directory_iterator(currDir)) {
			if (curr.is_directory())
				continue;
			std::filesystem::path relative = std::filesystem::relative(curr, currDir);
			currFileExtension = relative.extension().string().substr(1);
			if (currFileExtension == "mtl")
				continue;
			if (extensions.find(currFileExtension) == extensions.end())
				Error("File \"" + relative.string() + (std::string)"\" is not of supported type!");
			Log("Current file: " + relative.string());

			currFileContents = readFile(curr.path().string());
			if (currFileExtension == "obj") {
				std::filesystem::path mtlPath;
				for (const auto& curr2 : std::filesystem::recursive_directory_iterator(curr.path().parent_path())) {
					if (curr2.is_directory())
						continue;
					if (curr2.path().extension() != ".mtl")
						continue;
					std::string mtlFile = relative.filename().string().substr(0, relative.filename().string().size() - 4) + (std::string)".mtl";
					if (curr2.path().filename().string() == mtlFile) {
						mtlPath = curr2.path();
						break;
					}
				}
				if (mtlPath.empty())
					Error("No mtl file found for " + relative.filename().string() + (std::string)"!");
				Log("Found mtl file for " + relative.filename().string());
				std::set<std::string> neededImages;
				std::string mtlContents = readFile(mtlPath.string());
				getNeededImages(mtlContents, neededImages);
				if (!neededImages.empty()) {
					currFileContents.insert(0, "\n");
					for (std::string curr : neededImages) {
						currFileContents.insert(0, curr);
						currFileContents.insert(0, " ");
					}
					currFileContents = currFileContents.substr(1);
				}
				else {
					currFileContents.insert(0, "none\n");
				}
				currFileContents += '\n' + mtlContents;
			}
			for (std::string& currFilename : packFilenames)
				if (relative.filename().string() == currFilename)
					Error("Duplicate asset name \"" + relative.filename().string() + (std::string)"\" in pack!");
			packFilenames.push_back(relative.filename().string());
			if (out.rdbuf()->in_avail() != 0)
				out << "\n";
			out << "#NFASSET " + curr.path().filename().string();
			out << "\n";
			out << currFileContents;
			fileCount++;
		}
		if (fileCount == 0) {
			Log("No files found inside of \"" + currDir.path().filename().string() + (std::string)"\". No pack written.");
			continue;
		}
		writeFile(filename, out.str(), true);
		Success("Wrote \"" + filename + (std::string)"\" containing " + std::to_string(fileCount) + (std::string)" files.");
		dirCount++;
	}
	if (dirCount > 0)
		Log("Wrote " + std::to_string(dirCount) + (std::string)" asset pack(s).");
	else
		Log("No directories found!");
	Log("Done");
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return 0;
}