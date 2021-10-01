#include "Drawable.h"

#include "Utility.h"

namespace nf {
	Drawable::Drawable() :
		m_vao(nullptr),
		m_ib(nullptr)
	{

	}

	void Drawable::bind() {
		Error("Tried to bind an invalid object!");
	}

	unsigned int Drawable::getIndexCount() {
		return m_ib->getCount();
	}

	Drawable::~Drawable() {
		delete m_vao;
		delete m_ib;
	}
}