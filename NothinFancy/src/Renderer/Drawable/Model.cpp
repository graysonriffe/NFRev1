#include "Model.h"

#include "Utility.h"

namespace nf {
	Model::Model() {
	}

	void Model::create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount, const char* vertexShader, const char* fragmentShader) {
		m_vao = new VertexArray;
		m_vao->addBuffer(vertexBufferData, vertexBufferSize);
		m_vao->push<float>(2);
		//TODO: Change this to 3
		m_vao->finishBufferLayout();
		m_ib = new IndexBuffer(indexBufferData, indexBufferCount);
		if (vertexShader && fragmentShader)
			m_shader = new Shader(vertexShader, fragmentShader);
	}

	Drawable::DrawableType Model::identity() {
		return DrawableType::NF_GAME;
	}

	void Model::bind() {
		if (m_vao == nullptr)
			Error("Tried to bind uninitialized model!");
		m_vao->bind();
		m_ib->bind();
		if (m_shader) {
			m_shader->bind();
			//TODO: Set uniforms here such as position and texture
		}
	}

	bool Model::hasCustomShader() {
		if (m_shader)
			return true;
		return false;
	}

	Model::~Model() {

	}
}