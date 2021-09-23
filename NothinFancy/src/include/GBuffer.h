#pragma once
#include <vector>
#include <array>

namespace nf {
	class Entity;

	class GBuffer {
	public:
		GBuffer();

		void resize();
		void render(std::vector<Entity*>& entities);
		void bindTextures();

		~GBuffer();
	private:
		unsigned int m_FBO;
		std::array<unsigned int, 5> m_textures; //TODO: Check this number
		unsigned int m_depth;

		unsigned int m_width, m_height;
	};
}