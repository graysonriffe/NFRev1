#include "Model.h"

#include <map>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GL/glew.h"

#include "Texture.h"
#include "Shader.h"
#include "Utility.h"

namespace nf {
	Material::Material(const void* vb, const size_t vbSize, const void* tc, const size_t tcSize, const void* vn, const size_t vnSize, const void* ib, const unsigned int ibCount, ATexture* diffTex, Vec3& diffColor, ATexture* specTex, float shininess, ATexture* normalTex) {
		m_vao = new VertexArray();
		m_vao->addBuffer(vb, vbSize);
		m_vao->push<float>(3);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(tc, tcSize);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(vn, vnSize);
		m_vao->push<float>(3);
		m_vao->finishBufferLayout();
		m_ib = new IndexBuffer(ib, ibCount);
		if (diffTex) {
			m_hasDiffuse = true;
			if (diffTex->alreadyLoaded)
				m_diffuseTexture = diffTex->loadedTexture;
			else
				m_diffuseTexture = new Texture(diffTex);
		}
		m_diffColor = diffColor;
		if (specTex) {
			m_hasSpecular = true;
			if (specTex->alreadyLoaded)
				m_specularTexture = specTex->loadedTexture;
			else
				m_specularTexture = new Texture(specTex, true);
		}
		m_shininess = shininess;
		if (normalTex) {
			m_hasNormal = true;
			if (normalTex->alreadyLoaded)
				m_normalTexture = normalTex->loadedTexture;
			else
				m_normalTexture = new Texture(normalTex, true);
		}
	}

	void Material::render(Shader* shader, bool onlyDepth) {
		m_vao->bind();
		m_ib->bind();
		if (!onlyDepth) {
			if (m_hasDiffuse) {
				shader->setUniform("material.hasDiffuseTex", true);
				m_diffuseTexture->bind();
			}
			else {
				shader->setUniform("material.hasDiffuseTex", false);
				glm::vec3 color(m_diffColor.x, m_diffColor.y, m_diffColor.z);
				shader->setUniform("material.diffuseColor", color);
			}
			if (m_hasSpecular) {
				shader->setUniform("material.hasSpecTex", true);
				m_specularTexture->bind(1);
				shader->setUniform("material.specularTexture", 1);
			}
			else
				shader->setUniform("material.hasSpecTex", false);
			if (m_hasNormal) {
				shader->setUniform("material.hasNormTex", true);
				m_normalTexture->bind(2);
				shader->setUniform("material.normalTexture", 2);
			}
			else
				shader->setUniform("material.hasNormTex", false);
			shader->setUniform("material.specPower", m_shininess);
		}
		glDrawElements(GL_TRIANGLES, m_ib->getCount(), GL_UNSIGNED_INT, nullptr);
	}

	Material::~Material() {
		delete m_diffuseTexture;
		delete m_specularTexture;
		delete m_normalTexture;
	}

	Model::Model(AModel* model) :
		m_base(model->isBaseAsset)
	{
		std::string obj = model->data;
		unsigned int startMtlPos = obj.find("newmtl");
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
			unsigned int pos = mtl.find(line) + strlen(line);
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

			unsigned int pos = file.find(line) + strlen(line) + remove;
			file = file.substr(pos);
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
					unsigned int index = (mats[curr]->outVB.size() / 3) - 1;
					mats[curr]->outIB.push_back(index);
					vertexMap[currVertex] = index;
					mats[curr]->ibCount++;
				}
			}
		}

		for (auto& m : mats) {
			TempMaterial& curr = *m.second;
			ATexture* diff = nullptr;
			if (curr.diffuseTextureName.size())
				diff = model->neededTextures[curr.diffuseTextureName];
			ATexture* spec = nullptr;
			if (curr.specularTextureName.size())
				spec = model->neededTextures[curr.specularTextureName];
			ATexture* norm = nullptr;
			if (curr.normalTextureName.size())
				norm = model->neededTextures[curr.normalTextureName];
			m_materials.push_back(new Material(&curr.outVB[0], curr.outVB.size() * sizeof(float), &curr.outTC[0], curr.outTC.size() * sizeof(float), &curr.outVN[0], curr.outVN.size() * sizeof(float), &curr.outIB[0], curr.ibCount, diff, curr.diffuseColor, spec, curr.shininess, norm));

			delete m.second;
		}
	}

	void Model::render(Shader* shader, bool onlyDepth) {
		for (Material* curr : m_materials) {
			curr->render(shader, onlyDepth);
		}
	}

	bool Model::isBaseAsset() {
		return m_base;
	}

	Model::~Model() {
		for (Material* curr : m_materials) {
			delete curr;
		}
	}
}