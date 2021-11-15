#include "Renderer.h"

#include "GL/glew.h"
#include "GL\wglew.h"
#include "glm/glm.hpp"
#include "stb_image.h"

#include "Application.h"
#include "GBuffer.h"
#include "Shader.h"
#include "Light.h"
#include "Entity.h"
#include "Model.h"
#include "Cubemap.h"
#include "UIElement.h"
#include "Button.h"
#include "Camera.h"
#include "Utility.h"
#include "Texture.h"

namespace nf {
	Renderer::Renderer(Application* app) :
		m_app(app),
		m_gBuffer(nullptr),
		m_shadowMapFBO(0),
		m_directionalDepthTexSize(0),
		m_pointDepthTexSize(0),
		m_directionalShadowMap(0),
		m_cubemap(nullptr),
		m_fadeIn(false),
		m_fadeOut(false),
		m_fadeInOpacity(1.0f),
		m_fadeOutOpacity(0.0f),
		m_fadeText(true),
		m_fadeOutComplete(false)
	{
		m_hdc = GetDC(m_app->getWindow());
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};
		int pf = ChoosePixelFormat(m_hdc, &pfd);
		SetPixelFormat(m_hdc, pf, &pfd);
		m_hglrc = wglCreateContext(m_hdc);
		wglMakeCurrent(m_hdc, m_hglrc);
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			Error("Could not initialize GLEW!");
		const int attrib[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};
		wglDeleteContext(m_hglrc);
		m_hglrc = wglCreateContextAttribsARB(m_hdc, NULL, attrib);
		wglMakeCurrent(m_hdc, m_hglrc);
		//TODO: Configure V-Sync with a custom max FPS
		wglSwapIntervalEXT(0);
		Log("OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//Nvidia drivers take two buffer swaps to do the first flush for some reason
		SwapBuffers(m_hdc);
		SwapBuffers(m_hdc);

		loadBaseAssets();

		m_gBuffer = new GBuffer;

		m_directionalDepthTexSize = 4096;
		m_pointDepthTexSize = 2048;
		createShadowMaps();

		if (!m_app->hasCustomWindowIcon()) {
			ATexture& windowTex = *(ATexture*)m_baseAP["defaultwindowicon.png"];
			int width, height, nChannels;
			unsigned char* tex = stbi_load_from_memory((const unsigned char*)windowTex.data, (unsigned int)windowTex.size, &width, &height, &nChannels, 0);
			std::vector<unsigned char> pixels(width * height * 4);
			for (unsigned int i = 0; i < pixels.size() / 4; i++) {
				pixels[i * 4 + 0] = tex[i * 4 + 2];
				pixels[i * 4 + 1] = tex[i * 4 + 1];
				pixels[i * 4 + 2] = tex[i * 4 + 0];
				pixels[i * 4 + 3] = tex[i * 4 + 3];
			}
			stbi_image_free(tex);
			HICON windowIcon = CreateIcon(GetModuleHandle(NULL), width, height, 1, 32, NULL, &pixels[0]);
			m_app->setWindowIcon(windowIcon);
		}

		float quadVB[] = {
			-1.0, -1.0, 0.0, 0.0,
			1.0, -1.0, 1.0, 0.0,
			1.0, 1.0, 1.0, 1.0,
			-1.0, 1.0, 0.0, 1.0
		};
		unsigned int quadIB[] = {
			0, 1, 2,
			2, 3, 0
		};
		m_quadVAO = new VertexArray;
		m_quadVAO->addBuffer(quadVB, sizeof(quadVB));
		m_quadVAO->pushFloat(2);
		m_quadVAO->pushFloat(2);
		m_quadVAO->finishBufferLayout();
		m_quadIB = new IndexBuffer(quadIB, 6);
		m_loadingText.create("NFLoadingText", Vec2(0.025f, 0.044f), Vec3(0.7f));
	}

	void Renderer::setFade(bool in, bool out, bool text) {
		m_fadeIn = in;
		m_fadeOut = out;
		m_fadeText = text;
	}

	bool Renderer::isFadeOutComplete() {
		return m_fadeOutComplete;
	}

	void Renderer::render(Entity& in) {
		if (in.isConstructed() == false)
			Error("Tried to render Entity before being created!");
		m_lGame.push_back(&in);
		//TODO: Sort transparent objects by distance; Farthest first
	}
	void Renderer::render(UIElement& in) {
		if (in.isConstructed() == false)
			Error("Tried to render a UI element before being created!");
		m_lUI.push_back(&in);
	}
	void Renderer::render(Light& in) {
		if (in.isConstructed() == false)
			Error("Tried to render a light before being created!");
		m_lights.push_back(&in);
	}
	void Renderer::render(Cubemap& in) {
		if (in.isConstructed() == false)
			Error("Tried to render a cubemap before being created!");
		m_cubemap = &in;
	}

	void Renderer::doFrame(Camera* camera, float dT) {
		//Begin frame
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_app->getConfig().width, m_app->getConfig().height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)m_app->getConfig().width / (float)m_app->getConfig().height, 0.1f, 1000.0f);
		camera->bind(m_gBufferShader, m_lightingShader, m_cubemapShader);

		//First, fill the gBuffer with entities
		m_gBufferShader->setUniform("proj", proj);
		m_gBuffer->render(m_lGame, m_gBufferShader);

		//Light entities using the gBuffer
		size_t lightsRemaining = m_lights.size();
		if (!lightsRemaining) {
			m_quadVAO->bind();
			m_quadIB->bind();
			m_lightingShader->bind();
			m_gBuffer->bindTextures(m_lightingShader);
			glDrawElements(GL_TRIANGLES, m_quadIB->getCount(), GL_UNSIGNED_INT, nullptr);
		}
		unsigned int drawCount = 0;
		while (lightsRemaining > 0) {
			size_t currLightsDrawn;
			if (lightsRemaining > m_texSlots)
				currLightsDrawn = m_texSlots;
			else
				currLightsDrawn = lightsRemaining;
			lightsRemaining -= currLightsDrawn;
			m_lightingShader->setUniform("numberOfLights", (int)currLightsDrawn);
			if(drawCount == 0)
				m_lightingShader->setUniform("isContinued", false);
			else {
				m_lightingShader->setUniform("isContinued", true);
				glBlendFunc(GL_ONE, GL_ONE);
				glDepthFunc(GL_LEQUAL);
			}
			for (unsigned int i = 0; i < currLightsDrawn; i++)
				m_lights[i]->bind(m_lightingShader, i);
			renderShadowMaps(currLightsDrawn);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, m_app->getConfig().width, m_app->getConfig().height);
			m_quadVAO->bind();
			m_quadIB->bind();
			m_lightingShader->bind();
			m_gBuffer->bindTextures(m_lightingShader);
			glDrawElements(GL_TRIANGLES, m_quadIB->getCount(), GL_UNSIGNED_INT, nullptr);
			m_lights.erase(m_lights.begin(), m_lights.begin() + currLightsDrawn);
			drawCount++;
		}
		m_lGame.clear();
		m_lights.clear();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);

		//Draw the cubemap if one is currently set
		if (m_cubemap != nullptr) {
			m_cubemapShader->setUniform("proj", proj);
			m_cubemap->render(m_cubemapShader);
		}
		m_cubemap = nullptr;

		//Draw UI elements
		glDisable(GL_DEPTH_TEST);
		proj = glm::ortho(0.0f, (float)m_app->getConfig().width, 0.0f, (float)m_app->getConfig().height);
		for (UIElement* draw : m_lUI) {
			UIElement& curr = *draw;
			if (curr.identity() == "text") {
				m_textShader->setUniform("proj", proj);
				Text& text = *(Text*)draw;
				text.render(m_textShader, m_app->getConfig().width, m_app->getConfig().height);
				continue;
			}
			else if (curr.identity() == "texture") {
				m_uiTextureShader->setUniform("proj", proj);
				curr.render(m_uiTextureShader, m_app->getConfig().width, m_app->getConfig().height);
			}
			else if (curr.identity() == "button") {
				m_uiTextureShader->setUniform("proj", proj);
				Button& button = *(Button*)draw;
				button.render(m_uiTextureShader, m_app->getConfig().width, m_app->getConfig().height, m_app, m_textShader);
			}
		}
		m_lUI.clear();

		//Fade over everything when states change
		if (m_fadeIn) {
			m_fadeShader->setUniform("opacity", m_fadeInOpacity);
			m_quadVAO->bind();
			m_quadIB->bind();
			glDrawElements(GL_TRIANGLES, m_quadIB->getCount(), GL_UNSIGNED_INT, nullptr);
			if (m_fadeText) {
				m_textShader->setUniform("proj", proj);
				m_loadingText.setOpacity(m_fadeInOpacity);
				m_loadingText.render(m_textShader, m_app->getConfig().width, m_app->getConfig().height);
			}
			if (m_fadeInOpacity <= 0.0) {
				m_fadeIn = false;
				m_fadeInOpacity = 1.0;
				m_fadeOutComplete = false;
			}
			else
				m_fadeInOpacity -= 2.5f * dT;
		}
		else if (m_fadeOut) {
			m_fadeShader->setUniform("opacity", m_fadeOutOpacity);
			m_quadVAO->bind();
			m_quadIB->bind();
			glDrawElements(GL_TRIANGLES, m_quadIB->getCount(), GL_UNSIGNED_INT, nullptr);
			if (m_fadeText) {
				m_textShader->setUniform("proj", proj);
				m_loadingText.setOpacity(m_fadeOutOpacity);
				m_loadingText.render(m_textShader, m_app->getConfig().width, m_app->getConfig().height);
			}
			if (m_fadeOutOpacity >= 1.0) {
				m_fadeOut = false;
				m_fadeOutOpacity = 0.0;
				m_fadeOutComplete = true;
			}
			else
				m_fadeOutOpacity += 3.0f * dT;
		}
		glEnable(GL_DEPTH_TEST);

		//Check for OpenGL errors
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			Error("OpenGL error " + std::to_string(err));

		//Show completed frame
		SwapBuffers(m_hdc);
	}

	void Renderer::setAmbient(float am) {
		if (am < 0.0f)
			Error("Cannot have a negative ambient light strength!");
		m_lightingShader->setUniform("ambientStrength", am);
	}

	void Renderer::loadBaseAssets() {
		m_baseAP.load("base.nfpack");
		const char* gBufferVertex = m_baseAP.get("gBufferVertex.shader")->data;
		const char* gBufferFragment = m_baseAP.get("gBufferFragment.shader")->data;
		m_gBufferShader = new Shader(gBufferVertex, gBufferFragment);
		const char* lightingVertex = m_baseAP.get("lightingVertex.shader")->data;
		const char* lightingFragment = m_baseAP.get("lightingFragment.shader")->data;
		m_lightingShader = new Shader(lightingVertex, lightingFragment);
		const char* textVertex = m_baseAP.get("textVertex.shader")->data;
		const char* textFragment = m_baseAP.get("textFragment.shader")->data;
		m_textShader = new Shader(textVertex, textFragment);
		const char* uiTextureVertex = m_baseAP.get("uiTextureVertex.shader")->data;
		const char* uiTextureFragment = m_baseAP.get("uiTextureFragment.shader")->data;
		m_uiTextureShader = new Shader(uiTextureVertex, uiTextureFragment);
		const char* cubemapVertex = m_baseAP.get("cubemapVertex.shader")->data;
		const char* cubemapFragment = m_baseAP.get("cubemapFragment.shader")->data;
		m_cubemapShader = new Shader(cubemapVertex, cubemapFragment);
		const char* fadeVertex = m_baseAP.get("fadeVertex.shader")->data;
		const char* fadeFragment = m_baseAP.get("fadeFragment.shader")->data;
		m_fadeShader = new Shader(fadeVertex, fadeFragment);
		const char* directionalShadowVertex = m_baseAP.get("directionalShadowVertex.shader")->data;
		const char* directionalShadowFragment = m_baseAP.get("directionalShadowFragment.shader")->data;
		m_directionalShadowShader = new Shader(directionalShadowVertex, directionalShadowFragment);
		const char* pointShadowVertex = m_baseAP.get("pointShadowVertex.shader")->data;
		const char* pointShadowGeometry = m_baseAP.get("pointShadowGeometry.shader")->data;
		const char* pointShadowFragment = m_baseAP.get("pointShadowFragment.shader")->data;
		m_pointShadowShader = new Shader(pointShadowVertex, pointShadowFragment, pointShadowGeometry);

		BaseAssets::cube = (AModel*)m_baseAP.get("cube.obj");
		BaseAssets::plane = (AModel*)m_baseAP.get("plane.obj");
		BaseAssets::sphere = (AModel*)m_baseAP.get("sphere.obj");
		BaseAssets::cone = (AModel*)m_baseAP.get("cone.obj");
		BaseAssets::cylinder = (AModel*)m_baseAP.get("cylinder.obj");
		BaseAssets::torus = (AModel*)m_baseAP.get("torus.obj");
		BaseAssets::logo = (ATexture*)m_baseAP.get("logo.png");
		BaseAssets::cubemap = (ACubemap*)m_baseAP.get("default.cm");
		BaseAssets::font = (AFont*)m_baseAP.get("default.ttf");
		BaseAssets::button = (AButton*)m_baseAP.get("default.button");
	}

	void Renderer::createShadowMaps() {
		m_texSlots = 12;
		glGenFramebuffers(1, &m_shadowMapFBO);
		for (unsigned int i = 0; i < m_texSlots; i++) {
			unsigned int pointDepthMap;
			glGenTextures(1, &pointDepthMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthMap);
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_DEPTH_COMPONENT24, m_pointDepthTexSize, m_pointDepthTexSize);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			m_pointShadowMaps.push_back(pointDepthMap);
		}
		glGenTextures(1, &m_directionalShadowMap);
		glBindTexture(GL_TEXTURE_2D, m_directionalShadowMap);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, m_directionalDepthTexSize, m_directionalDepthTexSize);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Renderer::renderShadowMaps(size_t count) {
		float nearP = 0.1f, farP = 400.0f;
		glm::mat4 directionalLightProj = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, nearP, farP);
		glm::mat4 pointLightProj = glm::perspective(glm::radians(90.0f), 1.0f, nearP, farP);
		glm::mat4 lightView;
		glm::mat4 lightSpaceMat;
		bool directionalRendered = false;
		unsigned int directionalSlot = 0; //TODO: Test this
		glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
		for (unsigned int i = 0; i < count; i++) {
			Light::Type type = m_lights[i]->getType();
			unsigned int tex = type == Light::Type::DIRECTIONAL ? m_directionalShadowMap : m_pointShadowMaps[i];
			switch (type) {
			case Light::Type::DIRECTIONAL: {
				std::string stringPos;
				glViewport(0, 0, m_directionalDepthTexSize, m_directionalDepthTexSize);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glClear(GL_DEPTH_BUFFER_BIT);
				Vec3 posTemp = m_lights[i]->getPosition();
				glm::vec3 lightPos(posTemp.x, posTemp.y, posTemp.z);
				lightView = glm::lookAt(lightPos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
				lightSpaceMat = directionalLightProj * lightView;
				m_directionalShadowShader->setUniform("lightSpace", lightSpaceMat);
				stringPos = "lightSpaceMat[";
				stringPos += std::to_string(i);
				stringPos += "]";
				m_lightingShader->setUniform(stringPos, lightSpaceMat);

				instancedRenderShadows(m_directionalShadowShader);

				stringPos = "light[";
				stringPos += std::to_string(i);
				stringPos += "].directionalDepthTex";
				glActiveTexture(GL_TEXTURE4 + i);
				glBindTexture(GL_TEXTURE_2D, tex);
				m_lightingShader->setUniform(stringPos, 4 + (int)i);
				directionalRendered = true;
				break;
			}
			case Light::Type::POINT: {
				glViewport(0, 0, m_pointDepthTexSize, m_pointDepthTexSize);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glClear(GL_DEPTH_BUFFER_BIT);
				Vec3 posTemp = m_lights[i]->getPosition();
				glm::vec3 lightPos(posTemp.x, posTemp.y, posTemp.z);
				std::vector<glm::mat4> lightSpaceMats;
				lightSpaceMats.push_back(pointLightProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
				lightSpaceMats.push_back(pointLightProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
				lightSpaceMats.push_back(pointLightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
				lightSpaceMats.push_back(pointLightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
				lightSpaceMats.push_back(pointLightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
				lightSpaceMats.push_back(pointLightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
				for (int j = 0; j < 6; j++) {
					std::string stringPos = "lightSpaceMat[";
					stringPos += std::to_string(j);
					stringPos += "]";
					m_pointShadowShader->setUniform(stringPos, lightSpaceMats[j]);
				}
				m_pointShadowShader->setUniform("farPlane", farP);
				m_pointShadowShader->setUniform("lightPos", lightPos);

				instancedRenderShadows(m_pointShadowShader);

				std::string stringPos = "light[";
				stringPos += std::to_string(i);
				stringPos += "].pointDepthTex";
				glActiveTexture(GL_TEXTURE4 + i);
				glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
				m_lightingShader->setUniform(stringPos, 4 + (int)i);
				m_lightingShader->setUniform("farPlane", farP);
				break;
			}
			}
		}
		m_lightingShader->setUniform("numMats", (int)count);
	}

	void Renderer::instancedRenderShadows(Shader* shader) {
		std::unordered_map<Model*, std::vector<glm::mat4>> modelsToDraw;
		for (Entity* curr : m_lGame) {
			modelsToDraw[curr->getModel()].push_back(curr->getModelMatrix());
		}
		for (auto& curr : modelsToDraw) {
			std::vector<glm::mat4>& mats = curr.second;
			std::string pos;
			size_t modelsRemaining = mats.size();
			while (modelsRemaining != 0) {
				size_t modelCount;
				if (modelsRemaining > 60)
					modelCount = 60;
				else
					modelCount = modelsRemaining;
				modelsRemaining -= modelCount;
				for (unsigned int i = 0; i < modelCount; i++) {
					pos = std::to_string(i) + "]";
					shader->setUniform("model[" + pos, mats[i]);
				}
				curr.first->render(shader, true, (unsigned int)modelCount);
				mats.erase(mats.begin(), mats.begin() + modelCount);
			}
		}
	}

	Renderer::~Renderer() {
		delete m_gBufferShader;
		delete m_lightingShader;
		delete m_textShader;
		delete m_uiTextureShader;
		delete m_cubemapShader;
		delete m_fadeShader;
		delete m_directionalShadowShader;
		delete m_pointShadowShader;
		delete m_gBuffer;
		delete m_quadVAO;
		delete m_quadIB;
		ReleaseDC(m_app->getWindow(), m_hdc);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hglrc);
	}
}