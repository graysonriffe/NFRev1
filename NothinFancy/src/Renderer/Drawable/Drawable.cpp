#include "Drawable.h"

#include "Utility.h"

namespace nf {
	Drawable::Drawable() {

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