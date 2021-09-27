#include "Text.h"

#include "GL/glew.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "Application.h"
#include "UIElement.h"
#include "Shader.h"

namespace nf {
	Text::Text() :
		m_font(nullptr),
		m_scale(1.0f),
		m_opacity(1.0f)
	{

	}

	void Text::create(const std::string& string, const Vec2& position, const Vec3& color, double opacity, double scale, Asset* font) {
		if (m_constructed)
			Error("Text already created!");
		m_constructed = true;
		m_string = string;
		m_position = position;
		m_color = color;
		m_scale = (float)scale;
		m_opacity = (float)opacity;
		AFont& newFont = *(AFont*)font;
		if (newFont.alreadyLoaded) {
			m_font = newFont.loadedFont;
		}
		else {
			m_font = new Font;
			FT_Library ft;
			if (FT_Init_FreeType(&ft))
				Error("Could not initialize FreeType!");
			FT_Face face;
			if (FT_New_Memory_Face(ft, (const unsigned char*)newFont.data, newFont.size, 0, &face))
				Error("Could not load font!");
			FT_Set_Pixel_Sizes(face, 0, 160);
			for (unsigned char c = 0; c < 128; c++) {
				FT_Load_Char(face, c, FT_LOAD_RENDER);
				unsigned int tex;
				glGenTextures(1, &tex);
				glBindTexture(GL_TEXTURE_2D, tex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				Character ch = { tex, Vec2((float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows), Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top), (unsigned int)face->glyph->advance.x };
				m_font->m_characters[c] = ch;
			}
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			newFont.alreadyLoaded = true;
			newFont.loadedFont = m_font;
			m_font->isBase = newFont.isBaseAsset;
		}
		m_vao = new VertexArray;
		m_vao->addBuffer(nullptr, 0);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(nullptr, 0);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();

		if (m_string == "NFLoadingText") {
			m_string = "Loading...";
		}
		else {
			if (!Application::getApp()->getCurrentState()->isRunning())
				Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
		}
	}

	void Text::setText(const std::string& string) {
		m_string = string;
	}

	void Text::setColor(const Vec3& color) {
		m_color = color;
	}

	void Text::setScale(double scale) {
		m_scale = (float)scale;
	}

	void Text::setOpacity(double opacity) {
		m_opacity = (float)opacity;
	}

	const char* Text::identity() {
		return "text";
	}

	void Text::render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight, bool onButton, float buttonWidth, float buttonHeight, const Vec2& buttonPos) {
		float scale = windowWidth / 4000.0f;
		if (onButton)
			scale *= buttonWidth / 400.0f;
		float currX = (float)m_position.x * windowWidth, currY = (float)m_position.y * windowHeight;
		std::string::const_iterator si;
		if (m_centeredX || m_centeredY) {
			float textWidth = 0.0f;
			float textHeight = 0.0f;
			for (si = m_string.begin(); si != m_string.end(); si++) {
				Character& c = m_font->m_characters[*si];
				textWidth += (c.advance >> 6) * scale * m_scale;
				if (c.size.y >= textHeight)
					textHeight = (float)c.size.y * scale * m_scale;
			}
			if (m_centeredX)
				currX = ((float)windowWidth - textWidth) / 2;
			if (m_centeredY)
				currY = ((float)windowHeight - textHeight) / 2;

			if (onButton) {
				while (textWidth > buttonWidth - 20) {
					scale -= 0.01f;
					textHeight = textWidth = 0.0f;
					for (si = m_string.begin(); si != m_string.end(); si++) {
						Character& c = m_font->m_characters[*si];
						textWidth += (c.advance >> 6) * scale * m_scale;
						if (c.size.y >= textHeight)
							textHeight = (float)c.size.y * scale * m_scale;
					}
				}
				currX = (((float)buttonWidth - textWidth) / 2) + (float)buttonPos.x;
				currY = (((float)buttonHeight - textHeight) / 2) + (float)buttonPos.y;
			}
		}

		glm::vec3 color = { m_color.x, m_color.y, m_color.z };
		shader->setUniform("textColor", color);
		shader->setUniform("opacity", m_opacity);
		for (si = m_string.begin(); si != m_string.end(); si++) {
			Character& c = m_font->m_characters[*si];
			float x = currX + (float)c.bearing.x * scale * m_scale;
			float y = currY - float(c.size.y - c.bearing.y) * scale * m_scale;
			float w = (float)c.size.x * scale * m_scale;
			float h = (float)c.size.y * scale * m_scale;
			float vb[] = {
				x, y + h,
				x, y,
				x + w, y,
				x, y + h,
				x + w, y,
				x + w, y + h
			};
			float tc[] = {
				0.0, 0.0,
				0.0, 1.0,
				1.0, 1.0,
				0.0, 0.0,
				1.0, 1.0,
				1.0, 0.0
			};
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, c.texID);
			m_vao->bind();
			m_vao->setBufferData(0, vb, sizeof(vb));
			m_vao->setBufferData(1, tc, sizeof(tc));
			shader->setUniform("text", 4);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			currX += (c.advance >> 6) * scale * m_scale;
		}
	}

	void Text::destroy() {
		m_constructed = false;
		m_position = Vec2(0.0);
		m_centeredX = m_centeredY = false;
		m_string = std::string();
		if (!m_font->isBase)
			delete m_font;
		m_color = Vec3(0.0);
		m_scale = 1.0f;
		m_opacity = 1.0f;
	}

	Text::~Text() {
		destroy();
	}
}