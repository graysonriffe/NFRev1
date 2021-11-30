#include "nf/UITexture.h"

#include "GL/glew.h"

#include "nf/Application.h"
#include "nf/Assets.h"
#include "nf/Texture.h"
#include "nf/Shader.h"

namespace nf {
	UITexture::UITexture() :
		m_texture(nullptr),
		m_scale(1.0f),
		m_opacity(1.0f)
	{

	}

	void UITexture::create(Asset* textureAsset, const Vec2& position, float scale, float opacity) {
		if (m_constructed)
			NFError("UITexture already created!");
		m_constructed = true;
		ATexture* tex;
		if ((tex = dynamic_cast<ATexture*>(textureAsset)) == nullptr)
			NFError("Non-texture asset passed to UITexture::create!");
		m_position = position;
		m_scale = scale;
		m_opacity = opacity;
		if (tex->alreadyLoaded) {
			m_texture = tex->loadedTexture;
		}
		else {
			m_texture = new Texture(tex);
		}

		float tc[] = {
			0.0, 1.0,
			0.0, 0.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,
			1.0, 1.0
		};
		m_vao = new VertexArray;
		m_vao->addBuffer(nullptr, 0);
		m_vao->pushFloat(2);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(tc, sizeof(tc));
		m_vao->pushFloat(2);
		m_vao->finishBufferLayout();

		if (!Application::getApp()->getCurrentState()->isRunning())
			Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
	}

	const char* UITexture::identity() {
		return "texture";
	}

	void UITexture::setScale(float scale) {
		m_scale = scale;
	}

	void UITexture::setOpacity(float opacity) {
		m_opacity = opacity;
	}

	void UITexture::render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) {
		float posX = m_position.x * windowWidth, posY = m_position.y * windowHeight;
		float scale = windowWidth / 5.0f;

		Vec2 texDim = m_texture->getDimensions();
		float height = scale * m_scale;
		float width = (texDim.x / texDim.y) * scale * m_scale;
		if (m_centeredX)
			posX = ((float)windowWidth - width) / 2;
		if (m_centeredY)
			posY = ((float)windowHeight - height) / 2;
		float vb[3][4] = {
			posX, posY + height,
			posX, posY,
			posX + width, posY,
			posX, posY + height,
			posX + width, posY,
			posX + width, posY + height
		};

		m_texture->bind();
		m_vao->setBufferData(0, vb, sizeof(vb));
		shader->setUniform("opacity", m_opacity);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void UITexture::destroy() {
		if (m_constructed) {
			delete m_vao;
			m_vao = nullptr;
			if (!m_texture->isBaseAsset()) {
				delete m_texture;
				m_texture = nullptr;
			}
		}
		m_constructed = false;
		m_position = Vec2(0.0);
		m_centeredX = m_centeredY = false;
		m_scale = 1.0f;
		m_opacity = 1.0f;
	}

	UITexture::~UITexture() {
		destroy();
	}
}