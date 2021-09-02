#pragma once
#include <vector>
#include <Windows.h>

#include "Entity.h"
#include "Camera.h"

namespace nf {
	class Application;
	class UIElement;

	class Renderer {
	public:
		Renderer(Application* app);

		void render(Entity& in);
		void render(UIElement& in);
		//TODO: Create second render function for UIElements

		void doFrame(Camera* camera);

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		AssetPack baseAP;

		std::vector<Entity*> m_lGame;
		std::vector<UIElement*> m_lUI;
		Shader* m_entityShader;
		Shader* m_textShader;
		Shader* m_uiTextureShader;
	};
}