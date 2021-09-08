#include "Model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Utility.h"
#include "Texture.h"

namespace nf {
	Model::Model() :
		m_base(false),
		m_texture(nullptr)
	{

	}

	void Model::create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount, const void* normalsBufferData, size_t normalsBufferSize, const void* textureCoordinatesBufferData, size_t textureCoordinatesBufferSize, ATexture* texture) {
		m_vao = new VertexArray;
		m_vao->addBuffer(vertexBufferData, vertexBufferSize);
		m_vao->push<float>(3);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(textureCoordinatesBufferData, textureCoordinatesBufferSize);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(normalsBufferData, normalsBufferSize);
		m_vao->push<float>(3);
		m_vao->finishBufferLayout();
		if (texture->alreadyLoaded) {
			m_texture = texture->loadedTexture;
		}
		else {
			m_texture = new Texture;
			m_texture->create(texture);
			texture->alreadyLoaded = true;
			texture->loadedTexture = m_texture;
		}
		m_ib = new IndexBuffer(indexBufferData, indexBufferCount);
	}

	void Model::bind() {
		if (m_vao == nullptr)
			Error("Tried to bind uninitialized model!");
		m_vao->bind();
		if (m_texture)
			m_texture->bind();
		m_ib->bind();
	}

	void Model::setBaseAsset(bool isBase) {
		m_base = isBase;
	}

	bool Model::isBaseAsset() {
		return m_base;
	}

	Model::~Model() {
		delete m_texture;
	}
}