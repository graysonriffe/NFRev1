#include "nf/UIElement.h"

namespace nf {
	UIElement::UIElement() :
		m_constructed(false),
		m_centeredX(false),
		m_centeredY(false)
	{

	}

	const char* UIElement::identity() {
		return "none";
	}

	void UIElement::setPosition(float x, float y) {
		m_position = Vec2(x, y);
	}
	void UIElement::setPosition(const Vec2& position) {
		m_position = position;
	}

	void UIElement::centered(bool x, bool y) {
		m_centeredX = x;
		m_centeredY = y;
	}

	bool UIElement::isConstructed() {
		return m_constructed;
	}

	void UIElement::render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) {

	}

	UIElement::~UIElement() {

	}
}