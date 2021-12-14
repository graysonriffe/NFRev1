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
		m_base(model->isBaseAsset)
	{
		if (model->neededTextures.size() > 32)
			NFError("Model exceedes 32 texture limit!");

		std::string modelData(model->data, model->data + model->size);
		std::string mats = modelData.substr(0, modelData.find("/ENDMATERIALS") - 1);

		createMaterials(mats, model);

		unsigned int vboSizesStart = (unsigned int)modelData.find("/ENDMATERIALS") + 14;
		std::string vboSizes = modelData.substr(vboSizesStart, modelData.find("/ENDHEADER") - vboSizesStart - 1);

		unsigned int currPos = (unsigned int)modelData.find("/ENDHEADER") + 11;
		unsigned int currSize = 0;
		std::stringstream ss(vboSizes);

		float* pos;
		unsigned int posBuffSize;
		unsigned int* indices;
		unsigned int indicesCount;

		m_vao = new VertexArray;
		//Positions
		ss >> currSize;
		pos = (float*)&modelData[currPos];
		posBuffSize = currSize;
		m_vao->addBuffer(&modelData[currPos], currSize);
		m_vao->pushFloat(3);
		m_vao->finishBufferLayout();
		currPos += currSize;
		//Texture coordinates
		ss >> currSize;
		m_vao->addBuffer(&modelData[currPos], currSize);
		m_vao->pushFloat(2);
		m_vao->finishBufferLayout();
		currPos += currSize;
		//Normals
		ss >> currSize;
		m_vao->addBuffer(&modelData[currPos], currSize);
		m_vao->pushFloat(3);
		m_vao->finishBufferLayout();
		currPos += currSize;
		//Tangents
		ss >> currSize;
		m_vao->addBuffer(&modelData[currPos], currSize);
		m_vao->pushFloat(3);
		m_vao->finishBufferLayout();
		currPos += currSize;
		//Material indices
		ss >> currSize;
		m_vao->addBuffer(&modelData[currPos], currSize);
		m_vao->pushInt(1);
		m_vao->finishBufferLayout();
		currPos += currSize;
		//Vertex indices
		ss >> currSize;
		indices = (unsigned int*)&modelData[currPos];
		indicesCount = currSize;
		m_ib = new IndexBuffer(&modelData[currPos], currSize);

		if (physicsConvex)
			Application::getApp()->getPhysicsEngine()->addConvexMesh(this, pos, posBuffSize);
		else if(physicsTriangle)
			Application::getApp()->getPhysicsEngine()->addTriangleMesh(this, pos, posBuffSize, indices, indicesCount);
	}

	void Model::createMaterials(const std::string& mats, AModel* model) {
		std::stringstream ss(mats);
		std::string temp;
		std::getline(ss, temp);
		unsigned int matCount = std::stoi(temp);
		if (matCount > 32)
			NFError("Model exceedes 32 material limit!");

		for (unsigned int i = 0; i < matCount; i++) {
			Material curr;

			unsigned int texIndex;

			ss >> texIndex;
			if (texIndex != 0)
				curr.diffuse = new Texture(model->neededTextures[texIndex - 1]);
			else
				curr.diffuse = nullptr;
			ss >> texIndex;
			if (texIndex != 0)
				curr.specular = new Texture(model->neededTextures[texIndex - 1], true);
			else
				curr.specular = nullptr;
			ss >> texIndex;
			if (texIndex != 0)
				curr.normal = new Texture(model->neededTextures[texIndex - 1], true);
			else
				curr.normal = nullptr;

			ss >> curr.diffuseColor.x >> curr.diffuseColor.y >> curr.diffuseColor.z >> curr.shininess;

			m_materials.push_back(curr);
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
			if ((diff = m_materials[i].diffuse) != nullptr) {
				shader->setUniform(m_hasDiffString + currMatSuffix, true);
				diff->bind(texSlot);
				shader->setUniform(m_diffString + currMatSuffix, texSlot);
				texSlot++;
			}
			else {
				shader->setUniform(m_hasDiffString + currMatSuffix, false);
				glm::vec3 color(m_materials[i].diffuseColor.x, m_materials[i].diffuseColor.y, m_materials[i].diffuseColor.z);
				shader->setUniform(m_diffColorString + currMatSuffix, color);
			}
			Texture* spec;
			if ((spec = m_materials[i].specular) != nullptr) {
				shader->setUniform(m_hasSpecString + currMatSuffix, true);
				spec->bind(texSlot);
				shader->setUniform(m_specString + currMatSuffix, texSlot);
				texSlot++;
			}
			else
				shader->setUniform(m_hasSpecString + currMatSuffix, false);
			Texture* norm;
			if ((norm = m_materials[i].normal) != nullptr) {
				shader->setUniform(m_hasNormString + currMatSuffix, true);
				norm->bind(texSlot);
				shader->setUniform(m_normString + currMatSuffix, texSlot);
				texSlot++;
			}
			else
				shader->setUniform(m_hasNormString + currMatSuffix, false);
			shader->setUniform(m_specPowerString + currMatSuffix, m_materials[i].shininess);
		}
	}

	bool Model::isBaseAsset() {
		return m_base;
	}

	Model::~Model() {
		for (unsigned int i = 0; i < m_materials.size(); i++) {
			Texture* curr;
			if ((curr = m_materials[i].diffuse) != nullptr)
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(curr);
			if ((curr = m_materials[i].specular) != nullptr)
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(curr);
			if ((curr = m_materials[i].normal) != nullptr)
				Application::getApp()->getCurrentState()->m_texturesToDelete.insert(curr);
		}
	}
}