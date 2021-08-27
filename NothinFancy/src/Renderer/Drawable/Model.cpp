#include "Model.h"

#include "Utility.h"

namespace nf {
	Model::Model() {
	}

	void Model::create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount) {
		m_vao = new VertexArray;
		m_vao->addBuffer(vertexBufferData, vertexBufferSize);
		m_vao->push<float>(2);
		//TODO: Change this to 3
		m_vao->finishBufferLayout();
		m_ib = new IndexBuffer(indexBufferData, indexBufferCount);
	}

	void Model::bind() {
		if (m_vao == nullptr)
			Error("Tried to bind uninitialized model!");
		m_vao->bind();
		m_ib->bind();
	}

	Model::~Model() {

	}
}