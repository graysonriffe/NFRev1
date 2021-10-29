#include "Model.h"

#include <map>
#include <algorithm>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GL/glew.h"

#include "Application.h"
#include "Texture.h"
#include "Shader.h"
#include "Utility.h"

namespace nf {
	Model::Model(AModel* model, bool physicsExport) :
		m_base(model->isBaseAsset)
	{
		if (model->neededTextures.size() > 32)
			Error("Model exceedes 32 texture limit!");
		std::string obj = model->data;
		size_t startMtlPos = obj.find("newmtl");
		if (startMtlPos == std::string::npos)
			Error("No materials found in model!");
		std::string mtl = obj.substr(startMtlPos);
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
		std::unordered_map<std::string, TempMaterial*> mats;
		std::string currMat;
		while (true) {
			char line[500];
			int result = sscanf_s(mtl.c_str(), "\n%s", line, (unsigned)_countof(line));
			if (result == EOF)
				break;
			if (std::strcmp(line, "newmtl") == 0) {
				char matName[100];
				sscanf_s(mtl.c_str(), "\nnewmtl %s\n", matName, (unsigned)_countof(matName));
				currMat = matName;
				mats[currMat] = new TempMaterial;
			}
			else if (std::strcmp(line, "Kd") == 0) {
				float r = 0.0, g = 0.0, b = 0.0;
				sscanf_s(mtl.c_str(), "\nKd %f %f %f\n", &r, &g, &b);
				mats[currMat]->diffuseColor = Vec3(r, g, b);
			}
			else if (std::strcmp(line, "map_Kd") == 0) {
				char texName[100];
				sscanf_s(mtl.c_str(), "\nmap_Kd %s\n", texName, (unsigned)_countof(texName));
				mats[currMat]->diffuseTextureName = (std::string)texName;
			}
			else if (std::strcmp(line, "map_Ks") == 0) {
				char texName[100];
				sscanf_s(mtl.c_str(), "\nmap_Ks %s\n", texName, (unsigned)_countof(texName));
				mats[currMat]->specularTextureName = (std::string)texName;
			}
			else if (std::strcmp(line, "map_Bump") == 0) {
				char texName[100];
				sscanf_s(mtl.c_str(), "\nmap_Bump %s\n", texName, (unsigned)_countof(texName));
				mats[currMat]->normalTextureName = (std::string)texName;
			}
			else if (std::strcmp(line, "Ns") == 0) {
				float s = 0.0;
				sscanf_s(mtl.c_str(), "\nNs %f\n", &s);
				mats[currMat]->shininess = s;
			}
			size_t pos = mtl.find(line) + strlen(line);
			mtl = mtl.substr(pos);
		}

		std::string file = obj.substr(0, startMtlPos);
		std::vector<float> vbRaw, tcRaw, vnRaw;
		std::string usingMat;

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
				vbRaw.push_back(x);
				vbRaw.push_back(y);
				vbRaw.push_back(z);
			}
			else if (std::strcmp(line, "vt") == 0) {
				tcPresent = true;
				float u = 0.0, v = 0.0;
				sscanf_s(file.c_str(), "\nvt %f %f\n", &u, &v);
				remove = 18;
				tcRaw.push_back(u);
				tcRaw.push_back(v);
			}
			else if (std::strcmp(line, "vn") == 0) {
				vnPresent = true;
				float x = 0.0, y = 0.0, z = 0.0;
				sscanf_s(file.c_str(), "\nvn %f %f %f\n", &x, &y, &z);
				remove = 20;
				vnRaw.push_back(x);
				vnRaw.push_back(y);
				vnRaw.push_back(z);
			}
			else if (std::strcmp(line, "usemtl") == 0) {
				char matName[100];
				sscanf_s(file.c_str(), "\nusemtl %s\n", matName, (unsigned)_countof(matName));
				usingMat = matName;
				remove = 5;
			}
			else if (std::strcmp(line, "f") == 0) {
				unsigned int vertexIndex[3], uvIndex[3], vnIndex[3];
				sscanf_s(file.c_str(), "\nf %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &vnIndex[0], &vertexIndex[1], &uvIndex[1], &vnIndex[1], &vertexIndex[2], &uvIndex[2], &vnIndex[2]);
				remove = 15;
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

			size_t pos = file.find(line) + strlen(line) + remove;
			file.erase(0, pos);
		}

		if (!tcPresent)
			Error("No texture coordinates found in model!");
		if (!vnPresent)
			Error("No normals found in model!");

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
				glm::vec3 pos1(mats[curr]->unindexedVB[i * 9], mats[curr]->unindexedVB[i * 9 + 1], mats[curr]->unindexedVB[i * 9 + 2]);
				glm::vec2 uv1(mats[curr]->unindexedTC[i * 6], mats[curr]->unindexedTC[i * 6 + 1]);
				glm::vec3 pos2(mats[curr]->unindexedVB[i * 9 + 3], mats[curr]->unindexedVB[i * 9 + 4], mats[curr]->unindexedVB[i * 9 + 5]);
				glm::vec2 uv2(mats[curr]->unindexedTC[i * 6 + 2], mats[curr]->unindexedTC[i * 6 + 3]);
				glm::vec3 pos3(mats[curr]->unindexedVB[i * 9 + 6], mats[curr]->unindexedVB[i * 9 + 7], mats[curr]->unindexedVB[i * 9 + 8]);
				glm::vec2 uv3(mats[curr]->unindexedTC[i * 6 + 4], mats[curr]->unindexedTC[i * 6 + 5]);

				glm::vec3 edge1 = pos2 - pos1;
				glm::vec3 edge2 = pos3 - pos1;
				glm::vec2 delta1 = uv2 - uv1;
				glm::vec2 delta2 = uv3 - uv1;
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
			ATexture* diffA = nullptr;
			Texture* diff = nullptr;
			if (curr2.diffuseTextureName.size()) {
				diffA = model->neededTextures[curr2.diffuseTextureName];
				diff = new Texture(diffA);
			}
			ATexture* specA = nullptr;
			Texture* spec = nullptr;
			if (curr2.specularTextureName.size()) {
				specA = model->neededTextures[curr2.specularTextureName];
				spec = new Texture(specA, true);
			}
			ATexture* normA = nullptr;
			Texture* norm = nullptr;
			if (curr2.normalTextureName.size()) {
				normA = model->neededTextures[curr2.normalTextureName];
				norm = new Texture(normA, true);
			}
			m_materials.push_back(std::make_tuple(diff, spec, norm, curr2.diffuseColor.x, curr2.diffuseColor.y, curr2.diffuseColor.z, curr2.shininess));
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
		if (m_materials.size() > 32)
			Error("Model exceedes 32 material limit!");
		m_vao = new VertexArray;
		m_vao->addBuffer(&vboPositions[0], vboPositions.size() * sizeof(float));
		m_vao->pushFloat(3);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(&vboTexCoords[0], vboTexCoords.size() * sizeof(float));
		m_vao->pushFloat(2);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(&vboNormals[0], vboNormals.size() * sizeof(float));
		m_vao->pushFloat(3);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(&vboTangents[0], vboTangents.size() * sizeof(float));
		m_vao->pushFloat(3);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(&vboMaterialIndices[0], vboMaterialIndices.size() * sizeof(int));
		m_vao->pushInt(1);
		m_vao->finishBufferLayout();
		m_ib = new IndexBuffer(&vboIndices[0], vboIndices.size());

		if (physicsExport)
			Application::getApp()->getPhysicsEngine()->addMesh(this, vboPositions);
	}

	void Model::render(Shader* shader, bool onlyDepth, unsigned int count) {
		m_vao->bind();
		m_ib->bind();
		if (!onlyDepth)
			bindMaterials(shader);
#ifdef _DEBUG
		shader->validate();
#endif
		glDrawElementsInstanced(GL_TRIANGLES, m_ib->getCount(), GL_UNSIGNED_INT, nullptr, count);
	}

	void Model::bindMaterials(Shader* shader) {
		int texSlot = 0;
		for (unsigned int i = 0; i < m_materials.size(); i++) {
			std::string currMatSuffix = std::to_string(i) + (std::string)"]";
			Texture* diff;
			if ((diff = std::get<0>(m_materials[i])) != nullptr) {
				shader->setUniform(m_hasDiffString + currMatSuffix, true);
				diff->bind(texSlot);
				shader->setUniform(m_diffString + currMatSuffix, texSlot);
				texSlot++;
			}
			else {
				shader->setUniform(m_hasDiffString + currMatSuffix, false);
				glm::vec3 color(std::get<3>(m_materials[i]), std::get<4>(m_materials[i]), std::get<5>(m_materials[i]));
				shader->setUniform(m_diffColorString + currMatSuffix, color);
			}
			Texture* spec;
			if ((spec = std::get<1>(m_materials[i])) != nullptr) {
				shader->setUniform(m_hasSpecString + currMatSuffix, true);
				spec->bind(texSlot);
				shader->setUniform(m_specString + currMatSuffix, texSlot);
				texSlot++;
			}
			else
				shader->setUniform(m_hasSpecString + currMatSuffix, false);
			Texture* norm;
			if ((norm = std::get<2>(m_materials[i])) != nullptr) {
				shader->setUniform(m_hasNormString + currMatSuffix, true);
				norm->bind(texSlot);
				shader->setUniform(m_normString + currMatSuffix, texSlot);
				texSlot++;
			}
			else
				shader->setUniform(m_hasNormString + currMatSuffix, false);
			shader->setUniform(m_specPowerString + currMatSuffix, std::get<6>(m_materials[i]));
		}
	}

	bool Model::isBaseAsset() {
		return m_base;
	}

	Model::~Model() {
		for (unsigned int i = 0; i < m_materials.size(); i++) {
			Texture* curr;
			if ((curr = std::get<0>(m_materials[i])) != nullptr)
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(std::get<0>(m_materials[i]));
			if ((curr = std::get<1>(m_materials[i])) != nullptr)
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(std::get<1>(m_materials[i]));
			if ((curr = std::get<2>(m_materials[i])) != nullptr)
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(std::get<2>(m_materials[i]));
		}
	}
}