#include "UITexture.h"

#include "GL/glew.h"

#include "Assets.h"
#include "Texture.h"
#include "Shader.h"

namespace nf {
	UITexture::UITexture() :
		m_texture(nullptr),
		m_scale(1.0f),
		m_opacity(1.0f)
	{

	}

	void UITexture::create(Asset* textureAsset, const Vec2& position, double scale, double opacity) {
		m_constructed = true;
		ATexture* tex = (ATexture*)textureAsset;
		m_position = position;
		m_scale = (float)scale;
		m_opacity = (float)opacity;
		if (tex->alreadyLoaded) {
			m_texture = tex->loadedTexture;
		}
		else {
			m_texture = new Texture;
			m_texture->create(tex);
		}

		m_vao = new VertexArray;
		m_vao->addBuffer(nullptr, 0);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(nullptr, 0);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();
	}

	const char* UITexture::identity() {
		return "texture";
	}

	void UITexture::setScale(double scale) {
		m_scale = (float)scale;
	}

	void UITexture::setOpacity(double opacity) {
		m_opacity = (float)opacity;
	}

	void UITexture::render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) {
		float posX = (float)m_position.x * windowWidth, posY = (float)m_position.y * windowHeight;
		float scale = windowWidth / 5.0f;

		Vec2 texDim = m_texture->getDimensions();
		float height = scale * m_scale;
		float width = ((float)texDim.x / (float)texDim.y) * scale * m_scale;
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
		float tc[3][4] = {
			0.0, 1.0,
			0.0, 0.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,
			1.0, 1.0
		};
		m_texture->bind();
		m_vao->setBufferData(0, vb, sizeof(vb));
		m_vao->setBufferData(1, tc, sizeof(tc));
		shader->setUniform("opacity", m_opacity);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void UITexture::destroy() {
		m_constructed = false;
		m_position = Vec2(0.0);
		m_centeredX = m_centeredY = false;
		if (!m_texture->isBaseAsset())
			delete m_texture;
		m_scale = 1.0f;
		m_opacity = 1.0f;
		delete m_vao;
	}

	UITexture::~UITexture() {

	}
}