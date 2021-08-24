#pragma once
#include <vector>

#include "VertexBuffer.h"

namespace nf {
	struct VertexBufferElement {
		unsigned int type;
		unsigned int count;
		unsigned char normalized;
	};

	class VertexArray {
	public:
		VertexArray();

		void addBuffer(const void* data, const size_t size);
		template<typename T>
		void push(unsigned int count);
		void finishBufferLayout();
		void bind(unsigned int buffer = 1);

		~VertexArray();
	private:
		unsigned int m_id;
		bool m_lastBufferHasLayout;
		std::vector<VertexBuffer*> m_buffers;
		std::vector<VertexBufferElement> m_lastBufferLayout;
		unsigned int m_attribute;
		unsigned int m_lastStride;
	};
}