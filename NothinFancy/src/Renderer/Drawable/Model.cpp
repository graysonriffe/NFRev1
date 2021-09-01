#include "Model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Utility.h"
#include "Texture.h"

namespace nf {
	Model::Model() {
	}

	void Model::create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount, const void* textureCoordinatesBufferData, size_t textureCoordinatesBufferSize, const unsigned char* textureData, size_t textureSize) {
		m_vao = new VertexArray;
		m_vao->addBuffer(vertexBufferData, vertexBufferSize);
		m_vao->push<float>(3);
		m_vao->finishBufferLayout();
		if (textureCoordinatesBufferData && textureCoordinatesBufferSize && textureData) {
			m_vao->addBuffer(textureCoordinatesBufferData, textureCoordinatesBufferSize);
			m_vao->push<float>(2);
			m_vao->finishBufferLayout();
			m_texture = new Texture;
			m_texture->create(textureData, textureSize);
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

	Model::~Model() {
		delete m_texture;
	}
}