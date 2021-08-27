#include "IndexBuffer.h"

#include "GL/glew.h"

namespace nf {
	IndexBuffer::IndexBuffer(const void* data, size_t count) {
		m_count = count;
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	void IndexBuffer::bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	unsigned int IndexBuffer::getCount() {
		return m_count;
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_id);
	}
}