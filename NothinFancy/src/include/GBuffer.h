#pragma once
#include <vector>
#include <array>

namespace nf {
	class Entity;
	class Shader;

	class GBuffer {
	public:
		GBuffer();

		void render(std::vector<Entity*>& entities, Shader* shader);
		void bindTextures(Shader* shader);

		~GBuffer();
	private:
		void resize();
		unsigned int m_FBO;
		std::array<unsigned int, 4> m_textures;
		unsigned int m_depth;

		unsigned int m_width, m_height;
	};
}