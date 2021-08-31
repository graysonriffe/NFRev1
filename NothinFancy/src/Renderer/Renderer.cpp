#include "Renderer.h"

#include "GL/glew.h"
#include "GL\wglew.h"

#include "Application.h"
#include "Utility.h"
#include "resource.h"

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
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Win32Res vs(IDR_DEFAULTVERTEX);
		const char* defaultVertex = (const char*)vs.ptr;
		Win32Res fs(IDR_DEFAULTFRAGMENT);
		const char* defaultFragment = (const char*)fs.ptr;
		if (defaultVertex == nullptr || defaultFragment == nullptr)
			Error("Default engine resources not found! Please link nf.res to your application!");
		m_defaultShader = new Shader(defaultVertex, defaultFragment);
	}

	void Renderer::render(Entity& in) {
		if (&in == nullptr)
			Error("Tried to render Entity before being created!");
		m_lGame.push_back(&in);
	}

	void Renderer::doFrame() {
		glViewport(0, 0, m_app->getConfig().width, m_app->getConfig().height);
		proj = glm::perspective(glm::radians(45.0f), (float)m_app->getConfig().width / (float)m_app->getConfig().height, 0.1f, 100000.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (Entity* draw : m_lGame) {
			Entity& curr = *draw;
			curr.bind(m_defaultShader);
			m_defaultShader->setUniform("proj", proj);
			glDrawElements(GL_TRIANGLES, curr.getModel()->getIndexCount(), GL_UNSIGNED_INT, nullptr);
		}
		m_lGame.clear();

		for (Drawable* draw : m_lUI) {
			Drawable& curr = *draw;

		}
		m_lUI.clear();

		SwapBuffers(m_hdc);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			Error("OpenGL error " + std::to_string(err));
	}

	Renderer::~Renderer() {
		ReleaseDC(m_app->getWindow(), m_hdc);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hglrc);
	}
}