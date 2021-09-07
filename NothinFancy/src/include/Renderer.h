#pragma once
#include <vector>
#include <Windows.h>

#include "Assets.h"
#include "Text.h"

namespace nf {
	class Application;
	class Shader;
	class Entity;
	class UIElement;
	class Light;
	class Cubemap;
	class Camera;
	class VertexArray;
	class IndexBuffer;

	class Renderer {
	public:
		Renderer(Application* app);

		void setFade(bool in, bool out, bool noText);
		bool isFadeOutComplete();

		void render(Entity& in);
		void render(UIElement& in);
		void render(Light& in);
		void render(Cubemap& in);

		void doFrame(Camera* camera);

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		AssetPack m_baseAP;

		std::vector<Light*> m_lights;
		std::vector<Entity*> m_lGame;
		Cubemap* m_cubemap;
		std::vector<UIElement*> m_lUI;
		Shader* m_entityShader;
		Shader* m_textShader;
		Shader* m_uiTextureShader;
		Shader* m_cubemapShader;
		Shader* m_fadeShader;

		bool m_fadeIn, m_fadeOut;
		bool m_fadeNoText;
		bool m_fadeOutComplete;
		Text m_loadingText;
		VertexArray* m_fadeVAO;
		IndexBuffer* m_fadeIB;
	};
}