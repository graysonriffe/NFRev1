#pragma once

namespace nf {
	class IndexBuffer {
	public:
		IndexBuffer(const void* data, size_t count);

		void bind() const;
		unsigned int getCount();

		~IndexBuffer();
	private:
		unsigned int m_id;
		unsigned int m_count;
	};
}