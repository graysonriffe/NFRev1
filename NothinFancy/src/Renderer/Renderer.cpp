#include "Renderer.h"

#include "GL/glew.h"
#include "GL\wglew.h"
#include "glm/glm.hpp"

#include "Application.h"
#include "Shader.h"
#include "UIElement.h"
#include "Utility.h"

namespace nf {
	Renderer::Renderer(Application* app) {
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

		baseAP.load("base.nfpack");
		const char* entityVertex = baseAP["entityVertex.shader"]->data;
		const char* entityFragment = baseAP["entityFragment.shader"]->data;
		m_entityShader = new Shader(entityVertex, entityFragment);
		const char* textVertex = baseAP["textVertex.shader"]->data;
		const char* textFragment = baseAP["textFragment.shader"]->data;
		m_textShader = new Shader(textVertex, textFragment);
		const char* uiTextureVertex = baseAP["uiTextureVertex.shader"]->data;
		const char* uiTextureFragment = baseAP["uiTextureFragment.shader"]->data;
		m_uiTextureShader = new Shader(uiTextureVertex, uiTextureFragment);

		BaseAssets::cube = (AModel*)baseAP["cube.obj"];
		BaseAssets::plane = (AModel*)baseAP["plane.obj"];
		BaseAssets::sphere = (AModel*)baseAP["sphere.obj"];
		BaseAssets::cone = (AModel*)baseAP["cone.obj"];
		BaseAssets::cylinder = (AModel*)baseAP["cylinder.obj"];
		BaseAssets::torus = (AModel*)baseAP["torus.obj"];
		BaseAssets::defaultFont = (AFont*)baseAP["default.ttf"];
	}

	void Renderer::render(Entity& in) {
		if (&in == nullptr)
			Error("Tried to render Entity before being created!");
		m_lGame.push_back(&in);
		//TODO: Sort transparent objects by distance; Farthest first
	}
	void Renderer::render(UIElement& in) {
		if (&in == nullptr)
			Error("Tried to render Entity before being created!");
		m_lUI.push_back(&in);
		//TODO: Sort transparent objects by distance; Farthest first
	}

	void Renderer::doFrame(Camera* camera) {
		glViewport(0, 0, m_app->getConfig().width, m_app->getConfig().height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)m_app->getConfig().width / (float)m_app->getConfig().height, 0.1f, 100000.0f);
		for (Entity* draw : m_lGame) {
			Entity& curr = *draw;
			curr.bind(m_entityShader);
			m_entityShader->setUniform("proj", proj);
			camera->bind(m_entityShader);
			//TODO: Clean this up a bit
			glDrawElements(GL_TRIANGLES, curr.getModel()->getIndexCount(), GL_UNSIGNED_INT, nullptr);
		}
		m_lGame.clear();

		glDisable(GL_DEPTH_TEST);
		proj = glm::ortho(0.0f, (float)m_app->getConfig().width, 0.0f, (float)m_app->getConfig().height);
		for (UIElement* draw : m_lUI) {
			UIElement& curr = *draw;
			if (curr.identity() == "text") {
				m_textShader->bind();
				m_textShader->setUniform("proj", proj);
				curr.render(m_textShader, m_app->getConfig().width, m_app->getConfig().height);
				continue;
			}
			if (curr.identity() == "texture") {
				m_uiTextureShader->bind();
				m_uiTextureShader->setUniform("proj", proj);
				curr.render(m_uiTextureShader, m_app->getConfig().width, m_app->getConfig().height);
			}
		}
		m_lUI.clear();

		SwapBuffers(m_hdc);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			Error("OpenGL error " + std::to_string(err));
	}

	Renderer::~Renderer() {
		delete m_entityShader;
		delete m_textShader;
		delete m_uiTextureShader;
		ReleaseDC(m_app->getWindow(), m_hdc);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hglrc);
	}
}