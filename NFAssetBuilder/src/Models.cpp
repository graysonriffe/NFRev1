#include "Models.h"

#include <algorithm>
#include <sstream>
#include <filesystem>
#include <map>
#include <set>

void cookModel(std::string& OBJin, std::string& MTLin, std::string& out) {

	std::unordered_map<std::string, TempMaterial*> mats;
	parseMaterials(mats, MTLin);

	std::vector<float> vbRaw, tcRaw, vnRaw;
	std::string usingMat;

	bool tcPresent = false, vnPresent = false;
	size_t position = 0;

	std::stringstream ss(OBJin);
	std::string line;
	while (std::getline(ss, line)) {
		std::stringstream ss2(line);
		std::string firstWord;
		ss2 >> firstWord;

		if (std::strcmp(&firstWord[0], "v") == 0) {
			float x = 0.0f, y = 0.0f, z = 0.0f;
			ss2 >> x >> y >> z;
			vbRaw.push_back(x);
			vbRaw.push_back(y);
			vbRaw.push_back(z);
		}
		else if (std::strcmp(&firstWord[0], "vt") == 0) {
			tcPresent = true;
			float u = 0.0f, v = 0.0f;
			ss2 >> u >> v;
			tcRaw.push_back(u);
			tcRaw.push_back(v);
		}
		else if (std::strcmp(&firstWord[0], "vn") == 0) {
			vnPresent = true;
			float x = 0.0f, y = 0.0f, z = 0.0f;
			ss2 >> x >> y >> z;
			vnRaw.push_back(x);
			vnRaw.push_back(y);
			vnRaw.push_back(z);
		}
		else if (std::strcmp(&firstWord[0], "usemtl") == 0) {
			std::string matName;
			ss2 >> matName;
			usingMat = matName;
		}
		else if (std::strcmp(&firstWord[0], "f") == 0) {
			if (!tcPresent)
				Error("No texture coordinates found in model!");
			if (!vnPresent)
				Error("No normals found in model!");

			unsigned int vertexIndex[3], uvIndex[3], vnIndex[3];
			char temp;
			ss2 >> vertexIndex[0] >> temp >> uvIndex[0] >> temp >> vnIndex[0] >> vertexIndex[1] >> temp >> uvIndex[1] >> temp >> vnIndex[1] >> vertexIndex[2] >> temp >> uvIndex[2] >> temp >> vnIndex[2];
			if (ss2.rdbuf()->in_avail() > 1)
				Error("Model has non-triangle faces!");
			mats[usingMat]->vbIndices.push_back(vertexIndex[0]);
			mats[usingMat]->vbIndices.push_back(vertexIndex[1]);
			mats[usingMat]->vbIndices.push_back(vertexIndex[2]);
			mats[usingMat]->tcIndices.push_back(uvIndex[0]);
			mats[usingMat]->tcIndices.push_back(uvIndex[1]);
			mats[usingMat]->tcIndices.push_back(uvIndex[2]);
			mats[usingMat]->vnIndices.push_back(vnIndex[0]);
			mats[usingMat]->vnIndices.push_back(vnIndex[1]);
			mats[usingMat]->vnIndices.push_back(vnIndex[2]);
		}
	}

	for (auto& m : mats) {
		std::string curr = m.first;
		for (unsigned int i = 0; i < mats[curr]->vbIndices.size(); i++) {
			unsigned int vertexIndex = mats[curr]->vbIndices[i];
			unsigned int uvIndex = mats[curr]->tcIndices[i];
			unsigned int vnIndex = mats[curr]->vnIndices[i];
			float vertexX = vbRaw[(vertexIndex - 1) * 3];
			float vertexY = vbRaw[(vertexIndex - 1) * 3 + 1];
			float vertexZ = vbRaw[(vertexIndex - 1) * 3 + 2];
			float vertexU = tcRaw[(uvIndex - 1) * 2];
			float vertexV = tcRaw[(uvIndex - 1) * 2 + 1];
			float vnX = vnRaw[(vnIndex - 1) * 3];
			float vnY = vnRaw[(vnIndex - 1) * 3 + 1];
			float vnZ = vnRaw[(vnIndex - 1) * 3 + 2];
			mats[curr]->unindexedVB.push_back(vertexX);
			mats[curr]->unindexedVB.push_back(vertexY);
			mats[curr]->unindexedVB.push_back(vertexZ);
			mats[curr]->unindexedTC.push_back(vertexU);
			mats[curr]->unindexedTC.push_back(vertexV);
			mats[curr]->unindexedVN.push_back(vnX);
			mats[curr]->unindexedVN.push_back(vnY);
			mats[curr]->unindexedVN.push_back(vnZ);
		}

		for (unsigned int i = 0; i * 9 < mats[curr]->unindexedVB.size(); i++) {
			Vec3 pos1(mats[curr]->unindexedVB[i * 9], mats[curr]->unindexedVB[i * 9 + 1], mats[curr]->unindexedVB[i * 9 + 2]);
			Vec2 uv1(mats[curr]->unindexedTC[i * 6], mats[curr]->unindexedTC[i * 6 + 1]);
			Vec3 pos2(mats[curr]->unindexedVB[i * 9 + 3], mats[curr]->unindexedVB[i * 9 + 4], mats[curr]->unindexedVB[i * 9 + 5]);
			Vec2 uv2(mats[curr]->unindexedTC[i * 6 + 2], mats[curr]->unindexedTC[i * 6 + 3]);
			Vec3 pos3(mats[curr]->unindexedVB[i * 9 + 6], mats[curr]->unindexedVB[i * 9 + 7], mats[curr]->unindexedVB[i * 9 + 8]);
			Vec2 uv3(mats[curr]->unindexedTC[i * 6 + 4], mats[curr]->unindexedTC[i * 6 + 5]);

			Vec3 edge1 = pos2 - pos1;
			Vec3 edge2 = pos3 - pos1;
			Vec2 delta1 = uv2 - uv1;
			Vec2 delta2 = uv3 - uv1;
			float f = 1.0f / (delta1.x * delta2.y - delta2.x * delta1.y);
			float x = f * (delta2.y * edge1.x - delta1.y * edge2.x);
			float y = f * (delta2.y * edge1.y - delta1.y * edge2.y);
			float z = f * (delta2.y * edge1.z - delta1.y * edge2.z);
			mats[curr]->unindexedTan.push_back(x);
			mats[curr]->unindexedTan.push_back(y);
			mats[curr]->unindexedTan.push_back(z);
			mats[curr]->unindexedTan.push_back(x);
			mats[curr]->unindexedTan.push_back(y);
			mats[curr]->unindexedTan.push_back(z);
			mats[curr]->unindexedTan.push_back(x);
			mats[curr]->unindexedTan.push_back(y);
			mats[curr]->unindexedTan.push_back(z);
		}
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

	std::vector<float> vboPositions;
	std::vector<float> vboTexCoords;
	std::vector<float> vboNormals;
	std::vector<float> vboTangents;
	std::vector<int> vboMaterialIndices;
	std::vector<unsigned int> vboIndices;
	int matCount = 0;

	std::string modelHeader;
	std::vector<std::string> textures = getNeededTextures(MTLin);
	std::string textureHeader;

	if (textures.empty())
		textureHeader = "none\n";
	else
		for (const auto& curr : textures)
			textureHeader.append(curr + (std::string)"\n");

	modelHeader.append(textureHeader);
	modelHeader.append("/ENDTEXTURES\n");
	modelHeader.append(std::to_string(mats.size()) + (std::string)"\n");

	for (auto& m : mats) {
		std::string curr = m.first;
		std::map<Vertex, unsigned int> vertexMap;
		for (unsigned int i = 0; i * 3 < mats[curr]->unindexedVB.size(); i++) {
			Vertex currVertex = { mats[curr]->unindexedVB[(i * 3)], mats[curr]->unindexedVB[(i * 3) + 1], mats[curr]->unindexedVB[(i * 3) + 2], mats[curr]->unindexedTC[(i * 2)], mats[curr]->unindexedTC[(i * 2) + 1], mats[curr]->unindexedVN[(i * 3)], mats[curr]->unindexedVN[(i * 3) + 1], mats[curr]->unindexedVN[(i * 3) + 2] };
			bool found = false;
			found = vertexMap.find(currVertex) != vertexMap.end();
			if (found) {
				unsigned int index = vertexMap[currVertex];
				mats[curr]->outIB.push_back(index);
				mats[curr]->ibCount++;
			}
			else {
				mats[curr]->outVB.push_back(currVertex.x);
				mats[curr]->outVB.push_back(currVertex.y);
				mats[curr]->outVB.push_back(currVertex.z);
				mats[curr]->outTC.push_back(currVertex.u);
				mats[curr]->outTC.push_back(currVertex.v);
				mats[curr]->outVN.push_back(currVertex.vnX);
				mats[curr]->outVN.push_back(currVertex.vnY);
				mats[curr]->outVN.push_back(currVertex.vnZ);
				mats[curr]->outTan.push_back(mats[curr]->unindexedTan[(i * 3)]);
				mats[curr]->outTan.push_back(mats[curr]->unindexedTan[(i * 3 + 1)]);
				mats[curr]->outTan.push_back(mats[curr]->unindexedTan[(i * 3 + 2)]);
				size_t index = (mats[curr]->outVB.size() / 3) - 1;
				mats[curr]->outIB.push_back((unsigned int)index);
				vertexMap[currVertex] = (unsigned int)index;
				mats[curr]->ibCount++;
			}
		}

		TempMaterial& curr2 = *m.second;

		std::string textureIndices;
		if (!curr2.diffuseTextureName.empty())
			textureIndices.append(std::to_string((std::find(textures.begin(), textures.end(), curr2.diffuseTextureName) - textures.begin()) + 1) + (std::string)" ");
		else
			textureIndices.append(std::to_string(0) + (std::string)" ");
		if (!curr2.specularTextureName.empty())
			textureIndices.append(std::to_string((std::find(textures.begin(), textures.end(), curr2.specularTextureName) - textures.begin()) + 1) + (std::string)" ");
		else
			textureIndices.append(std::to_string(0) + (std::string)" ");
		if (!curr2.normalTextureName.empty())
			textureIndices.append(std::to_string((std::find(textures.begin(), textures.end(), curr2.normalTextureName) - textures.begin()) + 1) + (std::string)" ");
		else
			textureIndices.append(std::to_string(0) + (std::string)" ");

		modelHeader.append(textureIndices);
		modelHeader.append(std::to_string(curr2.diffuseColor.x) + (std::string)" ");
		modelHeader.append(std::to_string(curr2.diffuseColor.y) + (std::string)" ");
		modelHeader.append(std::to_string(curr2.diffuseColor.z) + (std::string)" ");
		modelHeader.append(std::to_string(curr2.shininess) + (std::string)"\n");

		size_t offset = vboPositions.size() / 3;
		vboPositions.insert(vboPositions.end(), curr2.outVB.begin(), curr2.outVB.end());
		vboTexCoords.insert(vboTexCoords.end(), curr2.outTC.begin(), curr2.outTC.end());
		vboNormals.insert(vboNormals.end(), curr2.outVN.begin(), curr2.outVN.end());
		vboTangents.insert(vboTangents.end(), curr2.outTan.begin(), curr2.outTan.end());
		vboMaterialIndices.insert(vboMaterialIndices.end(), curr2.outVB.size() / 3, matCount);
		if (offset)
			std::for_each(curr2.outIB.begin(), curr2.outIB.end(), [offset](unsigned int& out) { out += (unsigned int)offset; });
		vboIndices.insert(vboIndices.end(), curr2.outIB.begin(), curr2.outIB.end());
		delete m.second;
		matCount++;
	}
	modelHeader.append("/ENDMATERIALS\n");

	unsigned int posSize = (unsigned int)vboPositions.size() * sizeof(float);
	unsigned int tcSize = (unsigned int)vboTexCoords.size() * sizeof(float);
	unsigned int normSize = (unsigned int)vboNormals.size() * sizeof(float);
	unsigned int tanSize = (unsigned int)vboTangents.size() * sizeof(float);
	unsigned int matIndicesSize = (unsigned int)vboMaterialIndices.size() * sizeof(int);
	unsigned int indicesSize = (unsigned int)vboIndices.size() * sizeof(unsigned int);

	modelHeader.append(std::to_string(posSize) + (std::string)" ");
	modelHeader.append(std::to_string(tcSize) + (std::string)" ");
	modelHeader.append(std::to_string(normSize) + (std::string)" ");
	modelHeader.append(std::to_string(tanSize) + (std::string)" ");
	modelHeader.append(std::to_string(matIndicesSize) + (std::string)" ");
	modelHeader.append(std::to_string(vboIndices.size()) + (std::string)"\n");

	modelHeader.append("/ENDHEADER\n");

	out.append(modelHeader);
	out.append((char*)vboPositions.data(), posSize);
	out.append((char*)vboTexCoords.data(), tcSize);
	out.append((char*)vboNormals.data(), normSize);
	out.append((char*)vboTangents.data(), tanSize);
	out.append((char*)vboMaterialIndices.data(), matIndicesSize);
	out.append((char*)vboIndices.data(), indicesSize);
}

void parseMaterials(std::unordered_map<std::string, TempMaterial*>& mats, std::string& mtl) {
	std::string currMat;
	size_t position = 0;

	std::stringstream ss(mtl);
	std::string line;
	while (std::getline(ss, line)) {
		std::stringstream ss2(line);
		std::string firstWord;
		ss2 >> firstWord;

		if (std::strcmp(&firstWord[0], "newmtl") == 0) {
			ss2 >> currMat;
			mats[currMat] = new TempMaterial;
		}
		else if (std::strcmp(&firstWord[0], "Kd") == 0) {
			float r = 0.0f, g = 0.0f, b = 0.0f;
			ss2 >> r >> g >> b;
			mats[currMat]->diffuseColor = Vec3(r, g, b);
		}
		else if (std::strcmp(&firstWord[0], "map_Kd") == 0) {
			std::string texName = getTextureName(ss2);
			mats[currMat]->diffuseTextureName = texName;
		}
		else if (std::strcmp(&firstWord[0], "map_Ks") == 0) {
			std::string texName = getTextureName(ss2);
			mats[currMat]->specularTextureName = texName;
		}
		else if (std::strcmp(&firstWord[0], "map_Bump") == 0) {
			std::string texName = getTextureName(ss2);
			mats[currMat]->normalTextureName = texName;
		}
		else if (std::strcmp(&firstWord[0], "Ns") == 0) {
			float s = 0.0f;
			ss2 >> s;
			mats[currMat]->shininess = s;
		}
	}
}

std::vector<std::string> getNeededTextures(std::string mtl) {
	std::set<std::string> tex;
	while (mtl.size()) {
		size_t pos = mtl.find("map_");
		if (pos == std::string::npos)
			break;
		if (pos == mtl.find("map_Kd") || pos == mtl.find("map_Ks"))
			mtl = mtl.substr(pos + 7);
		else
			mtl = mtl.substr(pos + 9);
		std::stringstream ss(mtl);
		std::string temp = getNewLine(ss);
		size_t pos2 = temp.find_last_of("/\\");
		if (pos2 != std::string::npos)
			temp = temp.substr(pos2 + 1);
		tex.insert(temp);
	}

	std::vector<std::string> out;

	if (tex.empty())
		return out;
	else
		for (const auto& curr : tex)
			out.push_back(curr);
	return out;
}

std::string getTextureName(std::stringstream& ss) {
	std::string out;
	std::getline(ss, out);
	out = out.substr(out.find_last_of("\\/") + 1);

	return out;
}