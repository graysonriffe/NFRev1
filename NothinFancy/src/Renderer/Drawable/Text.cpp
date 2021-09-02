#include "Text.h"

#include "GL/glew.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "UIElement.h"
#include "Shader.h"

namespace nf {
	Text::Text() {

	}

	void Text::create(const std::string& string, const Vec2& position, const Vec3& color, unsigned int size, Asset* font) {
		m_string = string;
		m_position = position;
		m_color = color;
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			Error("Could not initialize FreeType!");
		FT_Face face;
		AFont& newFont = *(AFont*)font;
		if (newFont.alreadyLoaded) {
			m_font = newFont.loadedFont;
		}
		else {
			m_font = new Font;
			if (FT_New_Memory_Face(ft, (const unsigned char*)newFont.data, newFont.size, 0, &face))
				Error("Could not load font!");
			FT_Set_Pixel_Sizes(face, 0, size);
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
		}
		m_vao = new VertexArray;
		m_vao->addBuffer(nullptr, 0);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();
		m_vao->addBuffer(nullptr, 0);
		m_vao->push<float>(2);
		m_vao->finishBufferLayout();
	}

	void Text::setText(const std::string& string) {
		m_string = string;
	}

	const char* Text::identity() {
		return "text";
	}

	void Text::render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) {
		float scale = windowWidth / 4000.0f;
		m_vao->bind();
		std::string::const_iterator si;
		float currX = (float)m_position.x * windowWidth, currY = (float)m_position.y * windowHeight;
		if (m_centeredX || m_centeredY) {
			float textWidth = 0.0f;
			float textHeight = 0.0f;
			for (si = m_string.begin(); si != m_string.end(); si++) {
				Character& c = m_font->m_characters[*si];
				textWidth += (c.advance >> 6) * scale;
				if (c.size.y >= textHeight)
					textHeight = (float)c.size.y * scale;
			}
			if (m_centeredX)
				currX = ((float)windowWidth - textWidth) / 2;
			if (m_centeredY)
				currY = ((float)windowHeight - textHeight) / 2;
		}
		glm::vec3 color = { m_color.x, m_color.y, m_color.z };
		shader->setUniform("textColor", color);
		for (si = m_string.begin(); si != m_string.end(); si++) {
			Character& c = m_font->m_characters[*si];
			float x = currX + (float)c.bearing.x * scale;
			float y = currY - float(c.size.y - c.bearing.y) * scale;
			float w = (float)c.size.x * scale;
			float h = (float)c.size.y * scale;
			float vb[3][4] = {
				x, y + h,
				x, y,
				x + w, y,
				x, y + h,
				x + w, y,
				x + w, y + h
			};
			float tc[3][4] = {
				0.0, 0.0,
				0.0, 1.0,
				1.0, 1.0,
				0.0, 0.0,
				1.0, 1.0,
				1.0, 0.0
			};
			glBindTexture(GL_TEXTURE_2D, c.texID);
			m_vao->setBufferData(0, vb, sizeof(vb));
			m_vao->setBufferData(1, tc, sizeof(tc));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			currX += (c.advance >> 6) * scale;
		}
	}

	Text::~Text() {

	}
}