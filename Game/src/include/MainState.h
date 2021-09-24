#pragma once
#include "NothinFancy.h"

class MainState : public nf::Gamestate {
public:
	void onEnter() override;

	void update(double deltaTime) override;
	void render(nf::Renderer& renderer) override;

	void onExit() override;
private:
	nf::AssetPack ap;
	nf::Entity test;
	nf::Entity plane;
	nf::Text text;
	nf::UITexture uiTex;
	nf::Button button;
	nf::Button button2;
	nf::Light light;
	nf::Cubemap cm;

	nf::Sound sound;

	float xrot, yrot;

	std::vector<nf::Entity*> entities;
};