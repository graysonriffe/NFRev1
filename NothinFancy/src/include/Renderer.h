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

	/**
	 * @brief Handles rendering user objects
	 * 
	 * This should only be accessed in a gamestate's render function.
	 * 
	 * @todo Maybe move to something like rendering everything automatically and adding a
	 * show function to entities and UI stuff?
	*/
	class Renderer {
	public:
		/**
		 * @brief Renders an Entity
		 * @param in Entity to render
		 * 
		 * This function adds an Entity to the list of entites to render in the next frame.
		 * 
		 * @note An entity will participate in the physics simulation if it is setup to
		 * do so regardless of if it is rendered or not.
		*/
		void render(Entity& in);
		/**
		 * @brief Renders a UIElement
		 * @param in UIElement to render
		 * 
		 * This function adds a UIElement to the list UIElements to render in the next frame.
		*/
		void render(UIElement& in);
		/**
		 * @brief Renders a Light
		 * @param in Light to render
		 * 
		 * This function adds a Light to the list of lights to render in the next frame.
		*/
		void render(Light& in);
		/**
		 * @brief Renders a Cubemap
		 * @param in Cubemap to render
		 * 
		 * This function sets the next frame's Cubemap.
		 * 
		 * @note Because only one Cubemap can be rendered in a frame, only the last call to
		 * this function before a frame is produced will take effect.
		*/
		void render(Cubemap& in);
#ifndef NFIMPL
		void setFade(bool in, bool out, bool text = true);
		bool isFadeOutComplete();
		void doFrame(Camera* camera, float dT);
		void setAmbient(float am);

		Renderer(Application* app);
		~Renderer();
#endif
	private:
		void loadBaseAssets();
		void createShadowMaps();
		void renderShadowMaps(size_t count);
		void instancedRenderShadows(Shader* shader);

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
		float m_fadeInOpacity, m_fadeOutOpacity;
		bool m_fadeText;
		bool m_fadeOutComplete;
		Text m_loadingText;

		VertexArray* m_quadVAO;
		IndexBuffer* m_quadIB;
	};
}