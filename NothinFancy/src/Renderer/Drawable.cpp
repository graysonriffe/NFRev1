#include "Drawable.h"

namespace nf {
	Drawable::Drawable(const char* vertexShader, const char* fragmentShader, const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount) :
		m_shader(vertexShader, fragmentShader),
		m_vao(),
		m_ib(indexBufferData, indexBufferCount)
	{
		m_vao.addBuffer(vertexBufferData, vertexBufferSize);
		m_vao.push<float>(2);
		m_vao.finishBufferLayout();
	}

	unsigned int Drawable::getIndexCount() {
		return m_ib.getCount();
	}

	void Drawable::bind() {
		m_shader.bind();
		m_vao.bind();
		m_ib.bind();
	}

	Drawable::DrawableType Drawable::identity() {
		return DrawableType::NF_NONE;
	}

	Drawable::~Drawable() {

	}
}