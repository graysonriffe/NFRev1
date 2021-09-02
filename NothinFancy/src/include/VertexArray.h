#pragma once
#include <vector>

#include "VertexBuffer.h"

namespace nf {

	class VertexArray {
		struct VertexBufferElement {
			unsigned int type;
			unsigned int count;
			unsigned char normalized;
		};
	public:
		VertexArray();

		void addBuffer(const void* data, const size_t size);
		template<typename T>
		void push(unsigned int count);
		void finishBufferLayout();
		void bind();
		void setBufferData(unsigned int buffer, const void* data, const size_t dataSize);

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