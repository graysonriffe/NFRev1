#include "nf/Model.h"

#include <map>
#include <algorithm>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GL/glew.h"

#include "nf/Application.h"
#include "nf/Texture.h"
#include "nf/Shader.h"
#include "nf/Utility.h"

namespace nf {
	Model::Model(AModel* model, bool physicsConvex, bool physicsTriangle) :
		m_base(model->isBaseAsset),
		m_newMtl("newmtl"),
		m_newLine("\n")
	{
		if (model->neededTextures.size() > 32)
			NFError("Model exceedes 32 texture limit!");
		std::vector<char> file(model->data, model->data + model->size);
		size_t mtlPos = std::search(file.begin(), file.end(), m_newMtl, m_newMtl + std::strlen(m_newMtl)) - file.begin();
		std::vector<char> mtl(&file[mtlPos], &file[0] + file.size());

		std::unordered_map<std::string, TempMaterial*> mats;
		parseMaterials(mats, mtl);

		std::vector<char> obj(&file[0], &file[mtlPos]);
		std::vector<float> vbRaw, tcRaw, vnRaw;
		std::string usingMat;

		bool tcPresent = false, vnPresent = false;
		size_t position = 0;
		while (true) {
			size_t nextLine = std::search(obj.begin() + position, obj.end(), m_newLine, m_newLine + std::strlen(m_newLine)) - (obj.begin() + position);
			if (!nextLine)
				break;
			std::vector<char> line(&obj[position], &obj[position + nextLine]);
			position += nextLine + 1;
			std::stringstream ss(std::string(&line[0], line.size()));
			std::string firstWord;
			ss >> firstWord;

			if (std::strcmp(&firstWord[0], "v") == 0) {
				float x = 0.0f, y = 0.0f, z = 0.0f;
				ss >> x >> y >> z;
				vbRaw.push_back(x);
				vbRaw.push_back(y);
				vbRaw.push_back(z);
			}
			else if (std::strcmp(&firstWord[0], "vt") == 0) {
				tcPresent = true;
				float u = 0.0f, v = 0.0f;
				ss >> u >> v;
				tcRaw.push_back(u);
				tcRaw.push_back(v);
			}
			else if (std::strcmp(&firstWord[0], "vn") == 0) {
				vnPresent = true;
				float x = 0.0f, y = 0.0f, z = 0.0f;
				ss >> x >> y >> z;
				vnRaw.push_back(x);
				vnRaw.push_back(y);
				vnRaw.push_back(z);
			}
			else if (std::strcmp(&firstWord[0], "usemtl") == 0) {
				std::string matName;
				ss >> matName;
				usingMat = matName;
			}
			else if (std::strcmp(&firstWord[0], "f") == 0) {
				if (!tcPresent)
					NFError("No texture coordinates found in model!");
				if (!vnPresent)
					NFError("No normals found in model!");

				unsigned int vertexIndex[3], uvIndex[3], vnIndex[3];
				char temp;
				ss >> vertexIndex[0] >> temp >> uvIndex[0] >> temp >> vnIndex[0] >> vertexIndex[1] >> temp >> uvIndex[1] >> temp >> vnIndex[1] >> vertexIndex[2] >> temp >> uvIndex[2] >> temp >> vnIndex[2];
				if (ss.rdbuf()->in_avail() > 1)
					NFError("Model has non-triangle faces!");
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
			NFError("Model exceedes 32 material limit!");
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

		if (physicsConvex)
			Application::getApp()->getPhysicsEngine()->addConvexMesh(this, vboPositions);
		else if(physicsTriangle)
			Application::getApp()->getPhysicsEngine()->addTriangleMesh(this, vboPositions, vboIndices);
	}

	void Model::parseMaterials(std::unordered_map<std::string, TempMaterial*>& mats, std::vector<char>& mtl) {
		std::string currMat;
		size_t position = 0;
		while (true) {
			size_t nextLine = std::search(mtl.begin() + position, mtl.end(), m_newLine, m_newLine + std::strlen(m_newLine)) - (mtl.begin() + position);
			if (position + nextLine >= mtl.size())
				break;
			std::vector<char> line(&mtl[position], &mtl[position + nextLine]);
			position += nextLine + 1;
			std::stringstream ss(std::string(&line[0], line.size()));
			std::string firstWord;
			ss >> firstWord;

			if (std::strcmp(&firstWord[0], "newmtl") == 0) {
				std::string matName;
				ss >> matName;
				currMat = matName;
				mats[currMat] = new TempMaterial;
			}
			else if (std::strcmp(&firstWord[0], "Kd") == 0) {
				float r = 0.0f, g = 0.0f, b = 0.0f;
				ss >> r >> g >> b;
				mats[currMat]->diffuseColor = Vec3(r, g, b);
			}
			else if (std::strcmp(&firstWord[0], "map_Kd") == 0) {
				std::string texName;
				ss >> texName;
				mats[currMat]->diffuseTextureName = texName;
			}
			else if (std::strcmp(&firstWord[0], "map_Ks") == 0) {
				std::string texName;
				ss >> texName;
				mats[currMat]->specularTextureName = texName;
			}
			else if (std::strcmp(&firstWord[0], "map_Bump") == 0) {
				std::string texName;
				ss >> texName;
				mats[currMat]->normalTextureName = texName;
			}
			else if (std::strcmp(&firstWord[0], "Ns") == 0) {
				float s = 0.0f;
				ss >> s;
				mats[currMat]->shininess = s;
			}
		}
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
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(curr);
			if ((curr = std::get<1>(m_materials[i])) != nullptr)
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(curr);
			if ((curr = std::get<2>(m_materials[i])) != nullptr)
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(curr);
		}
	}
}