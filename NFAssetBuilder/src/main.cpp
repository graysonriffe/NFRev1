#include <iostream>
#include <thread>
#include <filesystem>
#include <set>

#include <Windows.h>
#include <compressapi.h>

#include "Utility.h"
#include "Models.h"

[[noreturn]]
void printHelp() {
	std::cout << "Nothin' Fancy Asset Builder\n\nThis tool creates .nfpack files for the NF engine to "
		"load at runtime. A pack gets created for each directory in the working directory if there "
		"are only compatible files inside. Subdirectories are not ignored.\n\nFor more information, "
		"refer to the Nothin' Fancy manual";

	std::exit(0);
}

int main(int argc, char* argv[]) {
	if (argc > 1 && ((std::string)argv[1] == "-h" || (std::string)argv[1] == "/?"))
		printHelp();

	Log("Starting up");

	initCompressor();

	std::set<std::string> extensions;
	extensions.insert({ "shader", "obj", "png", "jpg", "ttf", "wav", "ogg" });

	unsigned int dirCount = 0;

	const std::filesystem::path workingDir = std::filesystem::current_path();

	for (const auto& currDir : std::filesystem::directory_iterator(workingDir)) {
		if (!currDir.is_directory()) continue;
		std::string currPackFilename = currDir.path().filename().string().append(".nfpack");
		Log("Building pack " + currPackFilename);

		std::vector<std::string> packFilenames;
		std::vector<unsigned int> packFileSizes;
		std::string currFileExtension;
		std::string currFileIn;
		std::string currPackOut;
		unsigned int fileCount = 0;

		for (const auto& curr : std::filesystem::recursive_directory_iterator(currDir)) {
			if (curr.is_directory()) continue;

			std::filesystem::path relative = std::filesystem::relative(curr, currDir);
			std::string currFileName = relative.filename().string();

			for (auto& file : packFilenames)
				if (currFileName == file)
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
			else
				currFileOut = currFileIn;

			packFilenames.push_back(currFileName);
			packFileSizes.push_back((unsigned int)currFileOut.size());

			currPackOut += currFileOut;

			fileCount++;
		}

		if (!fileCount) {
			Log("No files found inside of \"" + currDir.path().filename().string() + (std::string)"\". No pack written.");
			continue;
		}
		else
			Log("Finished gathering files");

		//Write pack header with asset names and sizes
		std::string header;
		header += std::to_string(fileCount) + (std::string)"\n";
		for (unsigned int i = 0; i < packFilenames.size(); i++) {
			header += packFilenames[i];
			header += '\n';
			header += std::to_string(packFileSizes[i]);
			header += '\n';
		}
		header.append("/ENDHEADER");
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