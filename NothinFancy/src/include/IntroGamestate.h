#pragma once

#include "Gamestate.h"
#include "UITexture.h"
#include "Text.h"

namespace nf {
	class IntroGamestate : public Gamestate {
	public:
		void onEnter() override;

		void update(float deltaTime) override;
		void render(Renderer& renderer) override;

		void onExit() override;
	private:
		std::chrono::steady_clock::time_point m_start;
		float m_scale;
		UITexture m_logoTex;
		Text m_text;
	};
}