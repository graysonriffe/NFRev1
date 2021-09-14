#include "Renderer.h"

#include "GL/glew.h"
#include "GL\wglew.h"
#include "glm/glm.hpp"
#include "stb_image.h"

#include "Application.h"
#include "Shader.h"
#include "Light.h"
#include "Entity.h"
#include "Cubemap.h"
#include "UIElement.h"
#include "Button.h"
#include "Camera.h"
#include "Utility.h"

namespace nf {
	Renderer::Renderer(Application* app) :
		m_shadowMapFBO(0),
		m_directionalDepthTexSize(0),
		m_pointDepthTexSize(0),
		m_cubemap(nullptr),
		m_fadeIn(false),
		m_fadeOut(false),
		m_fadeNoText(false),
		m_fadeOutComplete(false)
	{
		m_app = app;
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
			0, 0
		};
		wglDeleteContext(m_hglrc);
		m_hglrc = wglCreateContextAttribsARB(m_hdc, NULL, attrib);
		wglMakeCurrent(m_hdc, m_hglrc);
		wglSwapIntervalEXT(0);
		Log("OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		loadBaseAssets();

		m_directionalDepthTexSize = 4096;
		m_pointDepthTexSize = 1024;
		createShadowMap();

		if (!m_app->isCustomWindowIcon()) {
			ATexture& windowTex = *(ATexture*)m_baseAP["defaultwindowicon.png"];
			int width, height, nChannels;
			unsigned char* tex = stbi_load_from_memory((const unsigned char*)windowTex.data, windowTex.size, &width, &height, &nChannels, 0);
			std::vector<unsigned char> pixels(width * height * 4);
			for (unsigned int i = 0; i < pixels.size() / 4; i++) {
				pixels[i * 4 + 0] = tex[i * 4 + 2];
				pixels[i * 4 + 1] = tex[i * 4 + 1];
				pixels[i * 4 + 2] = tex[i * 4 + 0];
				pixels[i * 4 + 3] = tex[i * 4 + 3];
			}
			HICON windowIcon = CreateIcon(GetModuleHandle(NULL), width, height, 1, 32, NULL, &pixels[0]);
			SendMessage(m_app->getWindow(), WM_SETICON, ICON_BIG, (LPARAM)windowIcon);
			SendMessage(m_app->getWindow(), WM_SETICON, ICON_SMALL, (LPARAM)windowIcon);
		}

		float fadeVB[] = {
			-1.0, -1.0,
			1.0, -1.0,
			1.0, 1.0,
			-1.0, 1.0
		};
		unsigned int fadeIB[] = {
			0, 1, 2,
			2, 3, 0
		};
		m_fadeVAO = new VertexArray;
		m_fadeVAO->addBuffer(fadeVB, sizeof(fadeVB));
		m_fadeVAO->push<float>(2);
		m_fadeVAO->finishBufferLayout();
		m_fadeIB = new IndexBuffer(fadeIB, 6);
		m_loadingText.create("NFLoadingText", Vec2(0.025, 0.044), Vec3(0.7, 0.7, 0.7));
	}

	void Renderer::setFade(bool in, bool out, bool noText) {
		m_fadeIn = in;
		m_fadeOut = out;
		m_fadeNoText = noText;
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

	void Renderer::doFrame(Camera* camera, double dT) {
		//Begin frame
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_app->getConfig().width, m_app->getConfig().height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera->bind(m_entityShader, m_cubemapShader);

		//Draw Entities (3D models)
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)m_app->getConfig().width / (float)m_app->getConfig().height, 0.1f, 10000.0f);
		m_entityShader->bind();
		m_entityShader->setUniform("proj", proj);
		for (Entity* draw : m_lGame) {
			Entity& curr = *draw;
			unsigned int drawCount = (unsigned int)std::ceil(m_lights.size() / (double)m_texSlots);
			if (drawCount == 0)
				drawCount++;
			unsigned int lightsRemaining = m_lights.size();
			int currLight;
			if (lightsRemaining > m_texSlots)
				currLight = -(int)m_texSlots;
			else
				currLight = -(int)lightsRemaining;
			for (unsigned int i = 0; i < drawCount; i++) {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				m_entityShader->setUniform("isContinued", false);
				if (i != 0) {
					glBlendFunc(GL_ONE, GL_ONE);
					glDepthFunc(GL_LEQUAL);
					m_entityShader->setUniform("isContinued", true);
				}
				unsigned int currLightsDrawn;
				if (lightsRemaining >= m_texSlots)
					currLightsDrawn = m_texSlots;
				else
					currLightsDrawn = lightsRemaining;
				lightsRemaining -= currLightsDrawn;
				currLight += (int)currLightsDrawn;
				m_entityShader->setUniform("numberOfLights", (int)currLightsDrawn);
				for (unsigned int j = 0; j < currLightsDrawn; j++) {
					m_lights[j + (unsigned int)currLight]->bind(m_entityShader, j);
				}
				renderShadowMaps(currLight, currLightsDrawn);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, m_app->getConfig().width, m_app->getConfig().height);
				curr.render(m_entityShader);
			}
			glDepthFunc(GL_LESS);
		}
		m_lGame.clear();
		m_lights.clear();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Draw cubemap where there isn't anything else
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

		if (m_fadeIn) {
			static double opacity = 1.0;
			m_fadeShader->setUniform("opacity", (float)opacity);
			m_fadeVAO->bind();
			m_fadeIB->bind();
			glDrawElements(GL_TRIANGLES, m_fadeIB->getCount(), GL_UNSIGNED_INT, nullptr);
			if (!m_fadeNoText) {
				m_textShader->setUniform("proj", proj);
				m_loadingText.setOpacity(opacity);
				m_loadingText.render(m_textShader, m_app->getConfig().width, m_app->getConfig().height);
			}
			if (dT > 1.0 / 60.0)
				dT = 1.0 / 60.0;
			opacity -= 2.5 * dT;
			if (opacity <= 0.0) {
				m_fadeIn = false;
				m_fadeOutComplete = false;
				opacity = 1.0;
			}
		}
		else if (m_fadeOut) {
			static double opacity = 0.0;
			m_fadeShader->setUniform("opacity", (float)opacity);
			m_fadeVAO->bind();
			m_fadeIB->bind();
			glDrawElements(GL_TRIANGLES, m_fadeIB->getCount(), GL_UNSIGNED_INT, nullptr);
			if (!m_fadeNoText) {
				m_textShader->setUniform("proj", proj);
				m_loadingText.setOpacity(opacity);
				m_loadingText.render(m_textShader, m_app->getConfig().width, m_app->getConfig().height);
			}
			opacity += 3.0 * dT;
			if (opacity >= 1.0) {
				m_fadeIn = false;
				m_fadeOutComplete = true;
				opacity = 0.0;
			}
		}
		glEnable(GL_DEPTH_TEST);

		SwapBuffers(m_hdc);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			Error("OpenGL error " + std::to_string(err));
	}

	void Renderer::renderShadowMaps(unsigned int startingLight, unsigned int count) {
		float nearP = 0.1f, farP = 400.0f;
		glm::mat4 directionalLightProj = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, nearP, farP);
		glm::mat4 pointLightProj = glm::perspective(glm::radians(90.0f), 1.0f, nearP, farP);
		glm::mat4 lightView;
		glm::mat4 lightSpaceMat;
		glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
		for (unsigned int i = 0; i < count; i++) {
			Light::Type type = m_lights[i]->getType();
			unsigned int tex = type == Light::Type::DIRECTIONAL ? m_directionalShadowMaps[i] : m_pointShadowMaps[i];
			switch (type) {
				case Light::Type::DIRECTIONAL: {
					glViewport(0, 0, m_directionalDepthTexSize, m_directionalDepthTexSize);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
					glClear(GL_DEPTH_BUFFER_BIT);
					Vec3 posTemp = m_lights[startingLight + i]->getPosition();
					glm::vec3 lightPos(posTemp.x, posTemp.y, posTemp.z);
					lightView = glm::lookAt(lightPos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
					lightSpaceMat = directionalLightProj * lightView;
					m_directionalShadowShader->setUniform("lightSpace", lightSpaceMat);
					std::string stringPos = "lightSpaceMat[";
					stringPos += std::to_string(i);
					stringPos += "]";
					m_entityShader->setUniform(stringPos, lightSpaceMat);
					for (Entity* curr : m_lGame) {
						curr->render(m_directionalShadowShader, true);
					}
					stringPos = "light[";
					stringPos += std::to_string(i);
					stringPos += "].directionalDepthTex";
					glActiveTexture(GL_TEXTURE3 + i);
					glBindTexture(GL_TEXTURE_2D, tex);
					m_entityShader->setUniform(stringPos, 3 + (int)i);
					break;
				}
				case Light::Type::POINT: {
					glViewport(0, 0, m_pointDepthTexSize, m_pointDepthTexSize);
					glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex, 0);
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
					glClear(GL_DEPTH_BUFFER_BIT);
					Vec3 posTemp = m_lights[startingLight + i]->getPosition();
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
					for (Entity* curr : m_lGame) {
						curr->render(m_pointShadowShader, true);
					}
					std::string stringPos = "light[";
					stringPos += std::to_string(i);
					stringPos += "].pointDepthTex";
					glActiveTexture(GL_TEXTURE3 + i);
					glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
					m_entityShader->setUniform(stringPos, 3 + (int)i);
					m_entityShader->setUniform("farPlane", farP);
					break;
				}
			}
		}
		m_entityShader->setUniform("numMats", (int)count);
	}

	void Renderer::loadBaseAssets() {
		m_baseAP.load("base.nfpack");
		const char* entityVertex = m_baseAP["entityVertex.shader"]->data;
		const char* entityFragment = m_baseAP["entityFragment.shader"]->data;
		m_entityShader = new Shader(entityVertex, entityFragment);
		const char* textVertex = m_baseAP["textVertex.shader"]->data;
		const char* textFragment = m_baseAP["textFragment.shader"]->data;
		m_textShader = new Shader(textVertex, textFragment);
		const char* uiTextureVertex = m_baseAP["uiTextureVertex.shader"]->data;
		const char* uiTextureFragment = m_baseAP["uiTextureFragment.shader"]->data;
		m_uiTextureShader = new Shader(uiTextureVertex, uiTextureFragment);
		const char* cubemapVertex = m_baseAP["cubemapVertex.shader"]->data;
		const char* cubemapFragment = m_baseAP["cubemapFragment.shader"]->data;
		m_cubemapShader = new Shader(cubemapVertex, cubemapFragment);
		const char* fadeVertex = m_baseAP["fadeVertex.shader"]->data;
		const char* fadeFragment = m_baseAP["fadeFragment.shader"]->data;
		m_fadeShader = new Shader(fadeVertex, fadeFragment);
		const char* directionalShadowVertex = m_baseAP["directionalShadowVertex.shader"]->data;
		const char* directionalShadowFragment = m_baseAP["directionalShadowFragment.shader"]->data;
		m_directionalShadowShader = new Shader(directionalShadowVertex, directionalShadowFragment);
		const char* pointShadowVertex = m_baseAP["pointShadowVertex.shader"]->data;
		const char* pointShadowGeometry = m_baseAP["pointShadowGeometry.shader"]->data;
		const char* pointShadowFragment = m_baseAP["pointShadowFragment.shader"]->data;
		m_pointShadowShader = new Shader(pointShadowVertex, pointShadowFragment, pointShadowGeometry);

		BaseAssets::cube = (AModel*)m_baseAP["cube.obj"];
		BaseAssets::plane = (AModel*)m_baseAP["plane.obj"];
		BaseAssets::sphere = (AModel*)m_baseAP["sphere.obj"];
		BaseAssets::cone = (AModel*)m_baseAP["cone.obj"];
		BaseAssets::cylinder = (AModel*)m_baseAP["cylinder.obj"];
		BaseAssets::torus = (AModel*)m_baseAP["torus.obj"];
		BaseAssets::logo = (ATexture*)m_baseAP["logo.png"];
		BaseAssets::cubemap = (ACubemap*)m_baseAP["default.cm"];
		BaseAssets::font = (AFont*)m_baseAP["default.ttf"];
		BaseAssets::button = (AButton*)m_baseAP["default.button"];
	}

	void Renderer::createShadowMap() {
		m_texSlots = 16;
		glGenFramebuffers(1, &m_shadowMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
		for (unsigned int i = 0; i < m_texSlots; i++) {
			unsigned int directionalDepthMap, pointDepthMap;
			glGenTextures(1, &directionalDepthMap);
			glGenTextures(1, &pointDepthMap);
			glBindTexture(GL_TEXTURE_2D, directionalDepthMap);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, m_directionalDepthTexSize, m_directionalDepthTexSize);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthMap);
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_DEPTH_COMPONENT24, m_pointDepthTexSize, m_pointDepthTexSize);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			m_directionalShadowMaps.push_back(directionalDepthMap);
			m_pointShadowMaps.push_back(pointDepthMap);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	Renderer::~Renderer() {
		delete m_entityShader;
		delete m_textShader;
		delete m_uiTextureShader;
		delete m_cubemapShader;
		delete m_fadeShader;
		delete m_directionalShadowShader;
		delete m_fadeVAO;
		delete m_fadeIB;
		ReleaseDC(m_app->getWindow(), m_hdc);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hglrc);
	}
}