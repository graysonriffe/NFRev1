#include <thread>
#include <filesystem>
#include <set>
#include <unordered_map>

#include <Windows.h>
#include <compressapi.h>

#include "Utility.h"
#include "Models.h"
#include "Textures.h"

int main(int argc, char* argv[]) {
	Log("Starting up");

	if (argc > 1) 
		if ((std::string)argv[1] == "-h") {
			Log("Nothin' Fancy Asset Builder\nThis tool creates .nfpack files for the NF engine to load "
				"at runtime.\nA pack gets created for each directory in the working directory if there "
				"are only compatible files inside.\nSubdirectories are not ignored.");
			return 0;
		}

	initCompressor();

	std::set<std::string> extensions;
	extensions.insert({ "shader", "obj", "png", "jpg", "ttf", "wav", "ogg" });

	unsigned int dirCount = 0;

	const std::filesystem::path workingDir = std::filesystem::current_path();

	for (const auto& currDir : std::filesystem::directory_iterator(workingDir)) {
		if (!currDir.is_directory()) continue;
		std::string currPackFilename = currDir.path().filename().string().append(".nfpack");
		Log("Building pack " + currPackFilename);

		std::unordered_map<std::string, size_t> packFiles;
		std::string currFileExtension;
		std::string currFileIn;
		std::string currPackOut;
		unsigned int fileCount = 0;

		for (const auto& curr : std::filesystem::recursive_directory_iterator(currDir)) {
			if (curr.is_directory()) continue;

			std::filesystem::path relative = std::filesystem::relative(curr, currDir);
			std::string currFileName = relative.filename().string();

			for (auto& file : packFiles)
				if (currFileName == file.first)
					Error("Duplicate asset \"" + currFileName + (std::string)"\" in pack!");

			currFileExtension = relative.extension().string().substr(1);
			if (currFileExtension == "mtl")
				continue;
			if (extensions.find(currFileExtension) == extensions.end())
				Error("File \"" + currFileName + (std::string)"\" is not of supported type!");

			Log("Current file: " + currFileName);

			currFileIn = readFile(curr.path().string());
			std::string currFileOut;

			if (currFileExtension == "obj") {
				std::string mtlFileName = currFileName.substr(0, currFileName.find_last_of('.')) + (std::string)".mtl";
				std::string mtl;
				for (const auto& curr2 : std::filesystem::recursive_directory_iterator(currDir)) {
					if (curr2.path().filename().string() == mtlFileName) {
						Log("Found mtl file for " + currFileName);
						mtl = readFile(curr2.path().string());
						break;
					}
				}
				if (mtl.empty())
					Error("No mtl file found for " + currFileName + (std::string)"!");
				Log("Cooking model...");
				cookModel(currFileIn, mtl, currFileOut);
			}
			else if (currFileExtension == "png" || currFileExtension == "jpg")
				cookTexture(currFileIn, currFileOut);

			packFiles[currFileName] = currFileOut.size();

			currPackOut += currFileOut;

			fileCount++;
		}

		if (!fileCount) {
			Log("No files found inside of \"" + currDir.path().filename().string() + (std::string)"\". No pack written.");
			continue;
		}
		else
			Log("Finished gathering files");

		std::string header;
		for (auto& currFile : packFiles) {
			header += currFile.first;
			header += ':';
			header.append((char*)&currFile.second, sizeof(size_t));
			header += ':';
		}
		header.erase(header.size() - 1);
		header.append("/NFENDOFPACKHEADER");
		int headerTableSize = (int)header.size();
		header.insert(0, (char*)&headerTableSize, sizeof(headerTableSize));
		header.insert(0, (char*)&fileCount, sizeof(fileCount));

		currPackOut.insert(0, header);

		writePack(currPackFilename, currPackOut);

		Success("Wrote \"" + currPackFilename + (std::string)"\" containing " + std::to_string(fileCount) + (std::string)" files.");

		dirCount++;
	}

	if (dirCount > 0)
		Log("Wrote " + std::to_string(dirCount) + (std::string)" asset pack(s).");
	else
		Log("No directories found!");

	Log("Done");
#ifdef _DEBUG
	std::this_thread::sleep_for(std::chrono::seconds(2));
#endif
	return 0;
}