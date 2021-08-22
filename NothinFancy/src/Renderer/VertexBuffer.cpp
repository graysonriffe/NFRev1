#include "VertexBuffer.h"

namespace nf {
	VertexBuffer::VertexBuffer(const void* data, size_t size) {
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	void VertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &m_id);
	}
}