#include "VertexArray.h"

#include "GL/glew.h"

#include "Utility.h"

namespace nf {
	VertexArray::VertexArray() {
		glGenVertexArrays(1, &m_id);
		glBindVertexArray(m_id);
		m_lastBufferHasLayout = true;
		m_attribute = 0;
		m_lastStride = 0;
	}

	void VertexArray::addBuffer(const void* data, const size_t size) {
		if (!m_lastBufferHasLayout)
			Error("Buffer added to vertex array has no layout!");
		m_buffers.push_back(new VertexBuffer(data, size));
		m_buffers.back()->bind();
		m_lastBufferHasLayout = false;
		m_lastStride = 0;
	}

	void VertexArray::pushFloat(unsigned int count) {
		if (m_lastBufferHasLayout)
			Error("Tried to modify a vertex array's buffer after the layout was final!");
		m_lastBufferLayout.push_back({ GL_FLOAT, count, GL_FALSE });
		m_lastStride += count * sizeof(GL_FLOAT);
	}

	void VertexArray::pushInt(unsigned int count) {
		if (m_lastBufferHasLayout)
			Error("Tried to modify a vertex array's buffer after the layout was final!");
		m_lastBufferLayout.push_back({ GL_INT, count, GL_FALSE });
		m_lastStride += count * sizeof(GL_INT);
	}

	void VertexArray::finishBufferLayout() {
		size_t offset = 0;
		for (; m_attribute < m_lastBufferLayout.size(); m_attribute++) {
			const VertexBufferElement& curr = m_lastBufferLayout[m_attribute];
			glEnableVertexAttribArray(m_attribute);
			if (curr.type != GL_INT)
				glVertexAttribPointer(m_attribute, curr.count, curr.type, curr.normalized, m_lastStride, (const void*)offset);
			else
				glVertexAttribIPointer(m_attribute, curr.count, curr.type, m_lastStride, (const void*)offset);
			offset += sizeof(curr.type) * curr.count;
		}
		m_lastBufferHasLayout = true;
		m_lastStride = 0;
	}

	void VertexArray::bind() {
		if (m_buffers.empty())
			Error("No buffers and layouts added to vertex array before being bound!");
		if (!m_lastBufferHasLayout)
			Error("Buffer added to vertex array has no layout!");
		glBindVertexArray(m_id);
	}

	void VertexArray::setBufferData(unsigned int buffer, const void* data, const size_t dataSize) {
		bind();
		m_buffers[buffer]->bind();
		glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	}

	VertexArray::~VertexArray() {
		for (VertexBuffer* curr : m_buffers) {
			delete curr;
		}
		glDeleteVertexArrays(1, &m_id);
	}
}