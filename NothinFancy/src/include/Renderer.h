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
	class GBuffer;
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

		void doFrame(Camera* camera, float dT);

		~Renderer();
	private:
		void renderShadowMaps(size_t count);

		void loadBaseAssets();
		void createShadowMaps();

		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		AssetPack m_baseAP;

		GBuffer* m_gBuffer;

		unsigned int m_shadowMapFBO;
		int m_directionalDepthTexSize;
		int m_pointDepthTexSize;
		unsigned int m_directionalShadowMap;
		std::vector<unsigned int> m_pointShadowMaps;
		unsigned int m_texSlots;

		std::vector<Light*> m_lights;
		std::vector<Entity*> m_lGame;
		Cubemap* m_cubemap;
		std::vector<UIElement*> m_lUI;
		Shader* m_gBufferShader;
		Shader* m_lightingShader;
		Shader* m_textShader;
		Shader* m_uiTextureShader;
		Shader* m_cubemapShader;
		Shader* m_fadeShader;
		Shader* m_directionalShadowShader;
		Shader* m_pointShadowShader;

		bool m_fadeIn, m_fadeOut;
		bool m_fadeNoText;
		bool m_fadeOutComplete;
		Text m_loadingText;

		VertexArray* m_quadVAO;
		IndexBuffer* m_quadIB;
	};
}