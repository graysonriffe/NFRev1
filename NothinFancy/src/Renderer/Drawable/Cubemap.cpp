#include "Cubemap.h"

#include <vector>
#include "GL/glew.h"
#include "stb_image.h"

#include "Assets.h"

namespace nf {
	Cubemap::Cubemap() :
		m_constructed(false),
		m_id(0)
	{

	}

	void Cubemap::create(Asset* cubemapAsset) {
		m_constructed = true;
		ACubemap& cm = *(ACubemap*)cubemapAsset;
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		struct CMFace {
			CMFace(unsigned char* d, unsigned int w, unsigned int h, unsigned int nc) :
				data(d),
				width(w),
				height(h),
				nChannels(nc)
			{

			}
			unsigned char* data;
			unsigned int width;
			unsigned int height;
			unsigned int nChannels;
		};
		std::vector<CMFace> faces;
		int tempWidth, tempHeight, tempNChannels;
		unsigned char* tempData;
		stbi_set_flip_vertically_on_load(false);
		tempData = stbi_load_from_memory((unsigned char*)cm.leftData, cm.leftSize, &tempWidth, &tempHeight, &tempNChannels, 0);
		faces.emplace_back(tempData, tempWidth, tempHeight, tempNChannels);
		tempData = stbi_load_from_memory((unsigned char*)cm.rightData, cm.rightSize, &tempWidth, &tempHeight, &tempNChannels, 0);
		faces.emplace_back(tempData, tempWidth, tempHeight, tempNChannels);
		stbi_set_flip_vertically_on_load(true);
		tempData = stbi_load_from_memory((unsigned char*)cm.topData, cm.topSize, &tempWidth, &tempHeight, &tempNChannels, 0);
		faces.emplace_back(tempData, tempWidth, tempHeight, tempNChannels);
		tempData = stbi_load_from_memory((unsigned char*)cm.bottomData, cm.bottomSize, &tempWidth, &tempHeight, &tempNChannels, 0);
		faces.emplace_back(tempData, tempWidth, tempHeight, tempNChannels);
		stbi_set_flip_vertically_on_load(false);
		tempData = stbi_load_from_memory((unsigned char*)cm.backData, cm.backSize, &tempWidth, &tempHeight, &tempNChannels, 0);
		faces.emplace_back(tempData, tempWidth, tempHeight, tempNChannels);
		tempData = stbi_load_from_memory((unsigned char*)cm.frontData, cm.frontSize, &tempWidth, &tempHeight, &tempNChannels, 0);
		faces.emplace_back(tempData, tempWidth, tempHeight, tempNChannels);
		for (unsigned int i = 0; i < faces.size(); i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, faces[i].width, faces[i].height, 0, faces[i].nChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, faces[i].data);
			stbi_image_free(faces[i].data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		float vb[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		m_vao = new VertexArray;
		m_vao->addBuffer(vb, sizeof(vb));
		m_vao->push<float>(3);
		m_vao->finishBufferLayout();
	}

	bool Cubemap::isConstructed() {
		return m_constructed;
	}

	void Cubemap::render() {
		m_vao->bind();
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		glDepthFunc(GL_LEQUAL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
	}

	Cubemap::~Cubemap() {
		if(m_constructed)
			glDeleteTextures(1, &m_id);
	}
}