#pragma once

namespace nf {
	class VertexBuffer {
	public:
		VertexBuffer(const void* data, const size_t size);

		void bind() const;

		~VertexBuffer();
	private:
		unsigned int m_id;
	};
}