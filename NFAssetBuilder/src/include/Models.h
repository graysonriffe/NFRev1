#pragma once
#include <unordered_map>
#include <vector>

#include "Utility.h"

struct TempMaterial {
	std::vector<float> outVB;
	std::vector<float> unindexedVB;
	std::vector<unsigned int> vbIndices;
	std::vector<float> outTC;
	std::vector<float> unindexedTC;
	std::vector<unsigned int> tcIndices;
	std::vector<float> outVN;
	std::vector<float> unindexedVN;
	std::vector<unsigned int> vnIndices;
	std::vector<float> unindexedTan;
	std::vector<float> outTan;
	std::vector<unsigned int> outIB;
	unsigned int ibCount = 0;

	std::string diffuseTextureName;
	Vec3 diffuseColor;
	std::string specularTextureName;
	std::string normalTextureName;
	float shininess = 1.0f;
};

void cookModel(std::string& in, std::string& MTLin, std::string& out);
void parseMaterials(std::unordered_map<std::string, TempMaterial*>& mats, std::string& mtl);
std::vector<std::string> getNeededTextures(std::string mtl);
std::string getTextureName(std::stringstream& ss);