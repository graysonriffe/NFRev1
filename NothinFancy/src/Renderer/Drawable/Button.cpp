#include "Button.h"

#include "GL/glew.h"

#include "Application.h"
#include "Input.h"
#include "Texture.h"
#include "Shader.h"

namespace nf {
	Button::Button() :
		m_idleTexture(nullptr),
		m_hoverTexture(nullptr),
		m_pressedTexture(nullptr),
		m_currentTexture(nullptr),
		m_scale(1.0f),
		m_opacity(1.0f),
		m_clicked(false),
		m_triggered(false)
	{

	}

	void Button::create(const Vec2& position, std::string string, Asset* buttonAsset, double scale, double opacity) {
		m_constructed = true;
		m_position = position;
		m_string = string;
		if (m_string.size()) {
			m_text.create(m_string, Vec2(0.0, 0.0));
			m_text.centered(true);
		}
		m_scale = (float)scale * 0.3f;
		m_opacity = (float)opacity;
		m_text.setOpacity(m_opacity);
		AButton* button;
		if ((button = dynamic_cast<AButton*>(buttonAsset)) == nullptr)
			Error("Non-button asset passed to Button::create!");
		m_idleTexture = new Texture(&button->idleTex);
		m_hoverTexture = new Texture(&button->hoverTex);
		m_pressedTexture = new Texture(&button->pressedTex);
		if (!((m_idleTexture->getDimensions() == m_hoverTexture->getDimensions()) && (m_idleTexture->getDimensions() == m_pressedTexture->getDimensions()))) {
			Error("Button images are not the same size!");
		}
		float tc[3][4] = {
			0.0, 1.0,
			0.0, 0.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,
			1.0, 1.0
		};
		m_vao = new VertexArray;
		m_vao->addBuffer(nullptr, 0);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(tc, sizeof(tc));
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();

		Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
	}

	const char* Button::identity() {
		return "button";
	}

	void Button::render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight, Application* app, Shader* textShader) {
		float posX = (float)m_position.x * windowWidth, posY = (float)m_position.y * windowHeight;
		float scale = windowWidth / 5.0f;

		Vec2 texDim = m_idleTexture->getDimensions();
		float height = scale * m_scale;
		float width = ((float)texDim.x / (float)texDim.y) * scale * m_scale;
		if (m_centeredX)
			posX = ((float)windowWidth - width) / 2;
		if (m_centeredY)
			posY = ((float)windowHeight - height) / 2;

		m_currentTexture = m_idleTexture;
		Vec2 mousePos = app->getMousePos();
		m_triggered = false;
		if (mousePos.x >= posX && mousePos.x <= posX + width && mousePos.y <= windowHeight - posY && mousePos.y >= (windowHeight - posY) - height) {
			m_currentTexture = m_hoverTexture;
			if (app->isMouse(NFI_LEFTMOUSE))
				m_clicked = true;
			else if (m_clicked) {
				m_triggered = true;
				m_clicked = false;
			}
		}
		else m_clicked = false;

		if (m_clicked)
			m_currentTexture = m_pressedTexture;
		float vb[] = {
			posX, posY + height,
			posX, posY,
			posX + width, posY,
			posX, posY + height,
			posX + width, posY,
			posX + width, posY + height
		};

		m_currentTexture->bind();
		m_vao->setBufferData(0, vb, sizeof(vb));
		shader->setUniform("opacity", m_opacity);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		if (m_string.size()) {
			Vec2 pos = Vec2(posX, posY);
			m_text.render(textShader, windowWidth, windowHeight, true, width, height, pos);
		}
	}

	bool Button::isClicked() {
		return m_triggered;
	}

	void Button::destroy() {
		m_constructed = false;
		m_clicked = false;
		m_position = Vec2(0.0);
		m_centeredX = m_centeredY = false;
		if (!m_idleTexture->isBaseAsset())
			delete m_idleTexture;
		if (!m_hoverTexture->isBaseAsset())
			delete m_hoverTexture;
		if (!m_pressedTexture->isBaseAsset())
			delete m_pressedTexture;
		delete m_vao;
	}

	Button::~Button() {

	}
}