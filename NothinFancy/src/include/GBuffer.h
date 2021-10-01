#pragma once
#include <vector>
#include <array>
#include <unordered_map>
#include "glm\glm.hpp"

namespace nf {
	class Entity;
	class Model;
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

		std::unordered_map<Model*, std::vector<glm::mat4>> m_modelsToDraw;
		const std::string m_modelString = "model[";
	};
}