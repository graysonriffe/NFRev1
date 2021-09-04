#pragma once
#include <vector>
#include <Windows.h>

#include "Assets.h"

namespace nf {
	class Application;
	class Shader;
	class Entity;
	class UIElement;
	class Light;
	class Camera;

	class Renderer {
	public:
		Renderer(Application* app);

		void render(Entity& in);
		void render(UIElement& in);
		void render(Light& in);

		void doFrame(Camera* camera);

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		AssetPack m_baseAP;

		std::vector<Light*> m_lights;
		std::vector<Entity*> m_lGame;
		std::vector<UIElement*> m_lUI;
		Shader* m_entityShader;
		Shader* m_textShader;
		Shader* m_uiTextureShader;
	};
}